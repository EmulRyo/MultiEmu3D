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
#include "../HW/ISMSScreenDrawable.h"
#include "../HW/Pad.h"
#include "../HW/Sound.h"
#include "../HW/Video.h"
#include "../HW/CPU.h"
#include "../HW/Pad.h"
#include "../HW/Debugger.h"
#include "Settings.h"
#include "EmulationThread.h"
#include "Joystick.h"
#include "RendererOGL.h"

using namespace std;

EmulationThread::EmulationThread()
{
    mutex = new wxMutex();
    
    m_screen = NULL;
    
	sound = new Sound();
    video = new Video(NULL);
    pad = new Pad();
	cpu = new CPU(video, pad, sound);
	cartridge = NULL;
    debugger = new Debugger(sound, video, cpu, cartridge, pad);
    
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
    m_soundEnabled = sound->GetEnabled();
    
    m_rewind.tail = -1;
    m_rewind.head = 0;
    m_rewind.length = 0;
    m_rewind.enabled = false;
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
    
    for (int i=0; i<m_rewind.length; i++) {
        delete m_rewind.data[i];
    }
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
                if (!m_rewind.enabled) {
                    cpu->ExecuteOneFrame();
                    
                    m_rewind.tail = (m_rewind.tail + 1) % MAX_REWINDS;
                    
                    if (m_rewind.length >= MAX_REWINDS) {
                        delete m_rewind.data[m_rewind.tail];
                        if (m_rewind.tail == m_rewind.head)
                            m_rewind.head = (m_rewind.head + 1) % MAX_REWINDS;
                    }
                    else
                        m_rewind.length++;
                    
                    m_rewind.data[m_rewind.tail] = new stringstream();
                    u8 *data = m_screen->GetBufferPtr();
                    int size = SMS_SCREEN_W*SMS_SCREEN_H*3;
                    m_rewind.data[m_rewind.tail]->write((char *)data, size);
                    cpu->SaveStateToRAM(m_rewind.data[m_rewind.tail]);
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
        if (cartridge)
            delete cartridge;
        
        wxString battsDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
        + wxT("Batts");
        
        if (!wxFileName::DirExists(battsDir))
            wxFileName::Mkdir(battsDir, 0777, wxPATH_MKDIR_FULL);
        
        battsDir += wxFileName::GetPathSeparator();
        
        if (isZip)
            cartridge = new Cartridge(string(fileName.mb_str()), string(battsDir.mb_str()), buffer, size);
        else
            cartridge = new Cartridge(string(fileName.mb_str()), string(battsDir.mb_str()));
        
        cpu->SetCartridge(cartridge);
        cpu->Reset();
        cpu->SetGGMode(gg);
        
        debugger = new Debugger(sound, video, cpu, cartridge, pad);
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
    
    PadSetKeys(SettingsGetInput1(), SettingsGetInput2());
    sound->ChangeSampleRate(SettingsGetSoundSampleRate());
    sound->SetEnabled(SettingsGetSoundEnabled());
}

void EmulationThread::SetScreen(ISMSScreenDrawable *screen)
{
    wxMutexLocker lock(*mutex);
    
    m_screen = screen;
    video->SetScreen(screen);
}

void EmulationThread::UpdateRewindScreen() {
    u8 *data = m_screen->GetBufferPtr();
    int size = SMS_SCREEN_W*SMS_SCREEN_H*3;
    stringstream *stream = m_rewind.data[m_rewind.visible];
    stream->seekg(0, stream->beg);
    stream->read((char *)data, size);
}

void EmulationThread::SetRewindPosition() {
    float value = -1.0f;
    
    if (m_rewind.enabled) {
        int tail = m_rewind.tail;
        if (m_rewind.head > m_rewind.tail)
            tail += MAX_REWINDS;
        int total = tail - m_rewind.head + 1;
        tail = m_rewind.tail;
        if (m_rewind.visible > m_rewind.tail)
            tail += MAX_REWINDS;
        value = 1.0f - ((float)(tail - m_rewind.visible) / total);
    }
    
    ((RendererBase *)m_screen)->SetRewindValue(value);
}

void EmulationThread::UpdatePad()
{
    if (emuState == Playing) {
        bool buttonsState[12];
        for (int i=0; i<12; i++)
            buttonsState[i] = wxGetKeyState(keysUsed[i]);
        joystick->UpdateButtonsState(buttonsState);
        
        if (m_rewind.enabled) {
            if (buttonsState[B1]) {
                m_rewind.enabled = false;
                cpu->LoadStateFromRAM(m_rewind.data[m_rewind.visible]);
                m_rewind.tail = m_rewind.visible;
                SetRewindPosition();
            }
            else if (buttonsState[B2]) {
                m_rewind.enabled = false;
                SetRewindPosition();
            }
            else if (buttonsState[LEFT]) {
                if (m_rewind.visible != m_rewind.head) {
                    m_rewind.visible = (m_rewind.visible + MAX_REWINDS-1) % MAX_REWINDS;
                    SetRewindPosition();
                    UpdateRewindScreen();
                }
            }
            else if (buttonsState[RIGHT]) {
                if (m_rewind.visible != m_rewind.tail) {
                    m_rewind.visible = (m_rewind.visible + 1) % MAX_REWINDS;
                    SetRewindPosition();
                    UpdateRewindScreen();
                }
            }
        }
        else {
            bool back = wxGetKeyState(WXK_BACK);
            
            if (!back) {
                wxMutexLocker lock(*mutex);
                pad->SetButtonsStatePad1(buttonsState);
                pad->SetButtonsStatePad2(&buttonsState[6]);
                pad->SetPauseState(wxGetKeyState(keysUsed[12]));
                
                bool space = wxGetKeyState(WXK_SPACE);
                SetSpeed(space ? SpeedMax : SpeedNormal);
            }
            else {
                m_rewind.enabled = true;
                ((RendererBase *)m_screen)->SetRewindValue(1.0f);
                m_rewind.visible = m_rewind.tail;
            }
        }
        //SetSpeed(SpeedMax);
    }
}

Debugger *EmulationThread::GetDebugger() {
    return debugger;
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
            m_soundEnabled = sound->GetEnabled();
            sound->SetEnabled(false);
        }
        else
            sound->SetEnabled(m_soundEnabled);
        m_speed = speed;
    }
}