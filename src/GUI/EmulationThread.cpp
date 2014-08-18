/*
 This file is part of DMGBoy.
 
 DMGBoy is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 DMGBoy is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with DMGBoy.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include "../HW/Pad.h"
#include "../HW/Sound.h"
#include "../HW/Video.h"
#include "../HW/CPU.h"
#include "../HW/Pad.h"
#include "../HW/Debugger.h"
#include "Settings.h"
#include "EmulationThread.h"
#include "Joystick.h"

using namespace std;

EmulationThread::EmulationThread()
{
    mutex = new wxMutex();
    
	sound = new Sound();
    video = new Video(NULL);
    pad = new Pad();
	cpu = new CPU(video, pad, sound);
	cartridge = NULL;
    debugger = new Debugger(sound, video, cpu, cartridge);
    
    keysUsed[0] = WXK_UP;
    keysUsed[1] = WXK_DOWN;
    keysUsed[2] = WXK_LEFT;
    keysUsed[3] = WXK_RIGHT;
    keysUsed[4] = (wxKeyCode)'A';
    keysUsed[5] = (wxKeyCode)'S';
    
    keysUsed[12] = WXK_RETURN;
    
    ApplySettings();
    
    SetState(NotStartedYet);
    
    joystick = new Joystick();
    m_finished = false;
}

EmulationThread::~EmulationThread() {
    emuState = Stopped;
	delete cpu;
	delete video;
	delete sound;
    delete pad;
    if (cartridge)
		delete cartridge;
    
    delete joystick;
}

void EmulationThread::SetState(enumEmuStates state)
{
    wxMutexLocker lock(*mutex);
    
    this->emuState = state;
    
    if (state == Playing)
        sound->SetEnabled(SettingsGetSoundEnabled());
    else
        sound->SetEnabled(false);
    
    if (state == Stopped)
    {
        if (cartridge)
            cartridge->Extract();
#ifdef MAKEGBLOG
        cpu->SaveLog();
#endif
        cpu->Reset();
    }
}

enumEmuStates EmulationThread::GetState()
{
    return this->emuState;
}

wxThread::ExitCode EmulationThread::Entry()
{
    while (!TestDestroy())
    {
        long desired = 15;  // Milisegundos deseados por frame
        // Deberia ser 16 pero con ese valor en linux el sonido se entrecorta
        
        swFrame.Start();
		{
			wxMutexLocker lock(*mutex);
			if (emuState == Playing)
            {
			    cpu->ExecuteOneFrame();
            }
		} // Desbloquear el mutex
        
        long time = swFrame.Time();
        
        if (time < desired)
            this->Sleep(desired-time);
    }
    
    m_finished = true;
    
    return 0;
}

bool EmulationThread::ChangeFile(wxString fileName)
{
    // Meter dentro de las llaves para que se desbloquee el mutex antes de
    // llamar a SetState
    {
        wxMutexLocker lock(*mutex);
        
        u8 *buffer = NULL;
        unsigned long size = 0;
        bool isZip = false;
        
        if (!wxFileExists(fileName))
        {
            wxMessageBox(_("The file:\n")+fileName+_("\ndoesn't exist"), _("Error"));
            return false;
        }
        
        wxString fileLower = fileName.Lower();
        if (fileLower.EndsWith(wxT(".zip")))
        {
            isZip = true;
            this->LoadZip(fileName, &buffer, &size);
            if ((buffer == NULL) || (size == 0))
                return false;
        }
        else if (!fileLower.EndsWith(wxT(".sms")))
        {
            wxMessageBox(_("Only sms and zip files allowed!"), _("Error"));
            return false;
        }
        
        // Si ha llegado aquí es que es un archivo permitido
        if (cartridge)
            delete cartridge;
        
        wxString battsDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
        + wxT("Batts");
        
        if (!wxFileName::DirExists(battsDir))
            wxFileName::Mkdir(battsDir, 0777, wxPATH_MKDIR_FULL);
        
        battsDir += wxFileName::GetPathSeparator();
        
        if (isZip)
            cartridge = new Cartridge(buffer, size, string(battsDir.mb_str()));
        else
            cartridge = new Cartridge(string(fileName.mb_str()), string(battsDir.mb_str()));
        
        cpu->LoadCartridge(cartridge);
        cpu->Reset();
        
        debugger = new Debugger(sound, video, cpu, cartridge);
    }
    
    
	SetState(Playing);
    
    return true;
}

/*
 * Carga un fichero comprimido con zip y busca una rom de gameboy (un fichero con extension gb o gbc).
 * Si existe mas de una rom solo carga la primera. Si se ha encontrado, la rom se devuelve en un buffer
 * junto con su tamaño, sino las variables se dejan intactas
 */
void EmulationThread::LoadZip(const wxString zipPath, u8 ** buffer, unsigned long * size)
{
	wxString fileInZip, fileLower;
	wxZipEntry* entry;
	wxFFileInputStream in(zipPath);
	wxZipInputStream zip(in);
	while ((entry = zip.GetNextEntry()))
	{
		fileInZip = entry->GetName();
        
		fileLower = fileInZip.Lower();
		if (fileLower.EndsWith(wxT(".sms")) || fileLower.EndsWith(wxT(".sms")))
		{
			*size = zip.GetSize();
			*buffer = new u8[*size];
			zip.Read(*buffer, *size);
			delete entry;
			return;
		}
		else
		{
			delete entry;
			continue;
		}
	}
    
	// Archivo no encontrado
	wxMessageBox(_("Master System rom not found in the file:\n")+zipPath, _("Error"));
	return;
}

void EmulationThread::LoadState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);
    
    cpu->LoadState(fileName, id);
}

void EmulationThread::SaveState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);
    
    cpu->SaveState(fileName, id);
}

void EmulationThread::ApplySettings()
{
    wxMutexLocker lock(*mutex);
    
    PadSetKeys(SettingsGetInput());
    sound->ChangeSampleRate(SettingsGetSoundSampleRate());
    sound->SetEnabled(SettingsGetSoundEnabled());
}

void EmulationThread::SetScreen(ISMSScreenDrawable * screen)
{
    wxMutexLocker lock(*mutex);

    video->SetScreen(screen);
}

void EmulationThread::UpdatePad()
{
    //wxMutexLocker lock(*mutex);
    if (emuState == Playing)
    {
        bool buttonsState[6];
        for (int i=0; i<6; i++)
            buttonsState[i] = wxGetKeyState(keysUsed[i]);
        joystick->UpdateButtonsState(buttonsState);
        
        pad->SetButtonsStatePad1(buttonsState);
        pad->SetPauseState(wxGetKeyState(keysUsed[12]));
    }
}

Debugger *EmulationThread::GetDebugger() {
    return debugger;
}

void EmulationThread::PadSetKeys(int* keys)
{
	for (int i=0; i<6; i++)
		keysUsed[i] = (wxKeyCode)keys[i];
}

bool EmulationThread::Finished() {
    return m_finished;
}
