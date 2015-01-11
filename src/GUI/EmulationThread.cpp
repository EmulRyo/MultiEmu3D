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
#include "../SMS-GG/SMS.h"
#include "Settings.h"
#include "EmulationThread.h"
#include "Joystick.h"
#include "RendererOGL.h"

using namespace std;
using namespace MasterSystem;

EmulationThread::EmulationThread()
{
    mutex = new wxMutex();
    
    m_screen = NULL;
    
    m_sms = new MasterSystem::SMS();
    
    keysUsed[0] = WXK_UP;
    keysUsed[1] = WXK_DOWN;
    keysUsed[2] = WXK_LEFT;
    keysUsed[3] = WXK_RIGHT;
    keysUsed[4] = (wxKeyCode)'A';
    keysUsed[5] = (wxKeyCode)'S';
    
    keysUsed[ 6] = (wxKeyCode)'I';
    keysUsed[ 7] = (wxKeyCode)'K';
    keysUsed[ 8] = (wxKeyCode)'J';
    keysUsed[ 9] = (wxKeyCode)'L';
    keysUsed[10] = (wxKeyCode)'G';
    keysUsed[11] = (wxKeyCode)'H';
    
    keysUsed[12] = WXK_RETURN;
    
    ApplySettings();
    
    SetState(NotStartedYet);
    
    joystick = new Joystick();
    m_finished = false;
    m_speed = SpeedNormal;
    m_soundEnabled = m_sms->SoundIsEnabled();
    
    m_rewind.SetCPU(m_sms->GetCPU());
}

EmulationThread::~EmulationThread() {
    emuState = Stopped;
    delete m_sms;
    
    delete joystick;
}

void EmulationThread::SetState(enumEmuStates state)
{
    wxMutexLocker lock(*mutex);
    
    this->emuState = state;
    
    if (state == Playing) {
        m_sms->SoundEnable(SettingsGetSoundEnabled());
        ((RendererBase *)m_screen)->SetIcon(Renderer::Play);
    }
    else
        m_sms->SoundEnable(false);
    
    if (state == Stopped)
    {
        ((RendererBase *)m_screen)->SetIcon(Renderer::Stop);
        ((RendererBase *)m_screen)->SetRewindValue(-1);
        m_rewind.Disable();
        
        m_sms->CartridgeExtract();
        m_sms->Reset();
    }
    else if (state == Paused) {
        ((RendererBase *)m_screen)->SetIcon(Renderer::Pause);
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
                if (!m_rewind.IsEnabled()) {
                    m_sms->ExecuteOneFrame();
                    m_rewind.AddFrame();
                }
            }
		} // Desbloquear el mutex
        
        if (m_speed == SpeedNormal) {
            long time = swFrame.Time();
            if (time < desired)
                this->Sleep(desired-time);
        }
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
        bool sms = fileLower.EndsWith(wxT(".sms"));
        bool gg  = fileLower.EndsWith(wxT(".gg"));
        if (fileLower.EndsWith(wxT(".zip")))
        {
            isZip = true;
            this->LoadZip(fileName, &buffer, &size, &gg);
            if ((buffer == NULL) || (size == 0))
                return false;
        }
        else if (!sms && !gg)
        {
            wxMessageBox(_("Only sms, gg and zip files allowed!"), _("Error"));
            return false;
        }
        
        // Si ha llegado aquí es que es un archivo permitido
        wxString battsDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
        + wxT("Batts");
        
        if (!wxFileName::DirExists(battsDir))
            wxFileName::Mkdir(battsDir, 0777, wxPATH_MKDIR_FULL);
        
        battsDir += wxFileName::GetPathSeparator();
        
        if (isZip)
            m_sms->CartridgeLoad(string(fileName.mb_str()), string(battsDir.mb_str()), buffer, size);
        else
            m_sms->CartridgeLoad(string(fileName.mb_str()), string(battsDir.mb_str()));
        
        m_sms->SetGGMode(gg);
    }
    
    
	SetState(Playing);
    
    return true;
}

/*
 * Carga un fichero comprimido con zip y busca una rom de gameboy (un fichero con extension gb o gbc).
 * Si existe mas de una rom solo carga la primera. Si se ha encontrado, la rom se devuelve en un buffer
 * junto con su tamaño, sino las variables se dejan intactas
 */
void EmulationThread::LoadZip(const wxString zipPath, u8 ** buffer, unsigned long *size, bool *gg)
{
	wxString fileInZip, fileLower;
	wxZipEntry* entry;
	wxFFileInputStream in(zipPath);
	wxZipInputStream zip(in);
	while ((entry = zip.GetNextEntry()))
	{
		fileInZip = entry->GetName();
        
		fileLower = fileInZip.Lower();
        bool sms = fileLower.EndsWith(wxT(".sms"));
        *gg = fileLower.EndsWith(wxT(".gg"));
		if (sms || *gg)
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
	wxMessageBox(_("Master System or Game Gear rom not found in the file:\n")+zipPath, _("Error"));
	return;
}

void EmulationThread::LoadState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);
    
    m_sms->LoadState(fileName, id);
}

void EmulationThread::SaveState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);
    
    m_sms->SaveState(fileName, id);
}

void EmulationThread::ApplySettings()
{
    wxMutexLocker lock(*mutex);
    
    PadSetKeys(SettingsGetInput1(), SettingsGetInput2());
    m_sms->SoundSetSampleRate(SettingsGetSoundSampleRate());
    m_sms->SoundEnable(SettingsGetSoundEnabled());
}

void EmulationThread::SetScreen(MasterSystem::ISMSScreenDrawable *screen)
{
    wxMutexLocker lock(*mutex);
    
    m_screen = screen;
    m_sms->SetScreen(screen);
    m_rewind.SetRenderer((RendererBase *)screen);
}

void EmulationThread::UpdatePad()
{
    if (emuState == Playing) {
        bool buttonsState[12];
        for (int i=0; i<12; i++)
            buttonsState[i] = wxGetKeyState(keysUsed[i]);
        joystick->UpdateButtonsState(buttonsState);
        bool back = wxGetKeyState(WXK_BACK);
        bool space = wxGetKeyState(WXK_SPACE);
        m_rewind.UpdatePad(buttonsState, back);
        
        if (!m_rewind.IsEnabled() && (!back)) {
            wxMutexLocker lock(*mutex);
            m_sms->PadSetButtons(buttonsState, wxGetKeyState(keysUsed[12]));
            
            SetSpeed(space ? SpeedMax : SpeedNormal);
        }
    }
}

MasterSystem::Debugger *EmulationThread::GetDebugger() {
    return m_sms->GetDebugger();
}

void EmulationThread::PadSetKeys(int* keys1, int* keys2) {
	for (int i=0; i<6; i++)
		keysUsed[i] = (wxKeyCode)keys1[i];
    for (int i=6; i<12; i++)
        keysUsed[i] = (wxKeyCode)keys2[i-6];
}

bool EmulationThread::Finished() {
    return m_finished;
}

void EmulationThread::SetSpeed(EnumSpeed speed) {
    if (m_speed != speed) {
        if (speed == SpeedMax) {
            m_soundEnabled = m_sms->SoundIsEnabled();
            m_sms->SoundEnable(false);
        }
        else
            m_sms->SoundEnable(m_soundEnabled);
        m_speed = speed;
    }
}