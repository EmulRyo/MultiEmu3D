/*
 This file is part of MultiEmu3D.
 
 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with MultiEmu3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include "../SMS-GG/SMS.h"
#include "../GB-GBC/GB.h"
#include "../NES/NES.h"
#include "../Common/VideoGameDevice.h"
#include "../Common/Exception.h"
#include "../Common/Utils.h"
#include "Settings.h"
#include "EmulationThread.h"
#include "Joystick.h"
#include "RendererOGL.h"
#include "Rewind.h"

using namespace std;

EmulationThread::EmulationThread()
{
    mutex = new wxMutex();
    
    m_screen = NULL;
    m_device = NULL;
    m_rewind = NULL;
    
    emuState = EmuState::NotStartedYet;
    
    joystick = new Joystick();
    m_finished = false;
    m_speed = EmuSpeed::Normal;
    for (int i = 0; i < 16; i++)
        keysUsed[i] = wxKeyCode::WXK_NONE;
    m_soundEnabled = false;
}

EmulationThread::~EmulationThread() {
    emuState = EmuState::Stopped;
    delete m_rewind;
    if (m_device)
        delete m_device;
    
    delete joystick;
}

void EmulationThread::SetState(EmuState state)
{
    wxMutexLocker lock(*mutex);
    
    if (!m_device)
        return;
    
    this->emuState = state;
    
    if (state == EmuState::Playing) {
        m_device->SoundEnable(SettingsGetSoundEnabled());
        ((RendererBase *)m_screen)->SetIcon(Renderer::Play);
    }
    else
        m_device->SoundEnable(false);
    
    if (state == EmuState::Stopped)
    {
        ((RendererBase *)m_screen)->SetIcon(Renderer::Stop);
        ((RendererBase *)m_screen)->SetRewindValue(-1);
        m_rewind->Disable();
        
        m_device->CartridgeExtract();
        m_device->Reset();
    }
    else if (state == EmuState::Paused) {
        ((RendererBase *)m_screen)->SetIcon(Renderer::Pause);
    }
}

EmuState EmulationThread::GetState()
{
    return this->emuState;
}

wxThread::ExitCode EmulationThread::Entry()
{
    const wxLongLong desired = 10000/60;  // Microsegundos deseados por frame
    wxLongLong accumulated = 0;
    int frames = 0;
    wxStopWatch fpsStopWatch;
    fpsStopWatch.Start();
    long lastFPS = 0;
    try {
        while (!TestDestroy())
        {
            swFrame.Start();
            {
                wxMutexLocker lock(*mutex);
                if (emuState == EmuState::Playing)
                {
                    if (!m_rewind->IsEnabled()) {
                        m_device->ExecuteOneFrame();
                        m_rewind->AddFrame();
                        frames++;
                    }
                }
            } // Desbloquear el mutex

            if (m_speed == EmuSpeed::Normal) {
                while ((swFrame.TimeInMicro() - accumulated) < desired) {
                    this->Sleep(1);
                }
                accumulated = swFrame.TimeInMicro() - accumulated - desired;
                //PrintfOutput("Time: %ld\n", accumulated);
            }
            
            long elapsed = fpsStopWatch.Time() - lastFPS;
            if (elapsed > 1000) {
                PrintfVisualStudioOutput("FPS: %f\n", (float)frames * 1000 / elapsed);
                lastFPS = fpsStopWatch.Time();
                frames = 0;
            }
             
        }
    }
    catch (Exception& exc) {
        wxMessageBox(exc.what());
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
        
        if (!wxFileExists(fileName)) {
            wxMessageBox(_("The file:\n")+fileName+_("\ndoesn't exist"), _("Error"));
            return false;
        }
        
        int dotPos = fileName.Find('.', true);
        if (dotPos == wxNOT_FOUND) {
            wxMessageBox(_("File without extension"), _("Error"));
            return false;
        }
        
        wxString extension = fileName.AfterLast('.').Lower();
        
        u8 *buffer = NULL;
        unsigned long size = 0;
        bool zip, sms, gb, nes;
        zip = sms = gb = nes = false;
        
        if (extension == "zip") {
            zip = true;
            LoadZip(fileName, &buffer, &size, extension);
            if ((buffer == NULL) || (size == 0))
                return false;
        }
        
        sms = MasterSystem::SMS::IsValidExtension(extension.ToStdString());
        gb  = GameBoy::GB::IsValidExtension(extension.ToStdString());
        nes = Nes::NES::IsValidExtension(extension.ToStdString());
        
        if (!sms && !gb && !nes) {
            wxMessageBox(_("Not valid files found!"), _("Error"));
            return false;
        }
        
        // Si ha llegado aquí es que es un archivo permitido
        wxString battsDir = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator()
        + wxT("Batts");
        
        if (!wxFileName::DirExists(battsDir))
            wxFileName::Mkdir(battsDir, 0777, wxPATH_MKDIR_FULL);
        
        battsDir += wxFileName::GetPathSeparator();
        
        if (m_device)
            delete m_device;
        
        if (sms)
            m_device = new MasterSystem::SMS();
        else if (gb)
            m_device = new GameBoy::GB();
        else if (nes)
            m_device = new Nes::NES();
        
        m_rewind = new Rewind(m_device);
        ApplySettingsNoMutex();
        SetScreenNoMutex(m_screen);
        
        if (zip)
            m_device->CartridgeLoad(string(fileName.mb_str()), string(battsDir.mb_str()), buffer, size);
        else
            m_device->CartridgeLoad(string(fileName.mb_str()), string(battsDir.mb_str()));
    }
    
	SetState(EmuState::Playing);
    
    return true;
}

/*
 * Carga un fichero comprimido con zip y busca una rom.
 * Si existe mas de una rom solo carga la primera. Si se ha encontrado, la rom se devuelve en un buffer
 * junto con su tamaño, sino las variables se dejan intactas
 */
void EmulationThread::LoadZip(const wxString &zipPath, u8 ** buffer, unsigned long *size, wxString &extension)
{
	wxString fileInZip, fileLower;
	wxZipEntry* entry;
	wxFFileInputStream in(zipPath);
	wxZipInputStream zip(in);
	while ((entry = zip.GetNextEntry()))
	{
		fileInZip = entry->GetName();
        
        int dotPos = fileInZip.Find('.', true);
        if (dotPos != wxNOT_FOUND) {
            extension = fileInZip.AfterLast('.').Lower();
            
            bool sms = MasterSystem::SMS::IsValidExtension(extension.ToStdString());
            bool gb  = GameBoy::GB::IsValidExtension(extension.ToStdString());
            bool nes = Nes::NES::IsValidExtension(extension.ToStdString());
            if (sms || gb || nes)
            {
                *size = zip.GetSize();
                *buffer = new u8[*size];
                zip.Read(*buffer, *size);
                delete entry;
                return;
            }
        }
		else
		{
			delete entry;
			continue;
		}
	}
    
	// Archivo no encontrado
	wxMessageBox(_("Not valid rom found in the file:\n")+zipPath, _("Error"));
	return;
}

void EmulationThread::LoadState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);
    
    m_device->LoadState(fileName, id);
}

void EmulationThread::SaveState(std::string fileName, int id)
{
    wxMutexLocker lock(*mutex);
    
    m_device->SaveState(fileName, id);
}

void EmulationThread::ApplySettings()
{
    wxMutexLocker lock(*mutex);
    
    ApplySettingsNoMutex();
}

void EmulationThread::ApplySettingsNoMutex()
{
    if (m_device) {
        PadSetKeys(SettingsGetInput(m_device->GetType()));
        m_device->SoundSetSampleRate(SettingsGetSoundSampleRate());
        m_device->SoundEnable(SettingsGetSoundEnabled());
    }
}

void EmulationThread::SetScreen(IScreenDrawable *screen) {
    wxMutexLocker lock(*mutex);
    
    SetScreenNoMutex(screen);
}

void EmulationThread::SetScreenNoMutex(IScreenDrawable *screen) {
    m_screen = screen;
    if (m_device)
        m_device->SetScreen(screen);
    if (m_rewind)
        m_rewind->SetRenderer((RendererBase *)screen);
}

void EmulationThread::UpdatePad()
{
    if (emuState == EmuState::Playing) {
        int numButtons = m_device->PadGetNumButtons();
        bool *buttonsState = new bool[numButtons];
        for (int i=0; i<numButtons; i++)
            buttonsState[i] = wxGetKeyState(keysUsed[i]);
        joystick->UpdateButtonsState(buttonsState);
        bool back = wxGetKeyState(WXK_BACK);
        bool space = wxGetKeyState(WXK_SPACE);
        m_rewind->UpdatePad(buttonsState, back);
        
        if (!m_rewind->IsEnabled() && (!back)) {
            wxMutexLocker lock(*mutex);
            m_device->PadSetButtons(buttonsState);
            
            SetSpeed(space ? EmuSpeed::Max : EmuSpeed::Normal);
        }
		delete[] buttonsState;
    }
}

VideoGameDevice *EmulationThread::GetVideoGameDevice() {
    return m_device;
}

void EmulationThread::PadSetKeys(int* keys) {
    for (int i=0; i<m_device->PadGetNumButtons(); i++)
		keysUsed[i] = (wxKeyCode)keys[i];
}

bool EmulationThread::Finished() {
    return m_finished;
}

void EmulationThread::SetSpeed(EmuSpeed speed) {
    if (m_speed != speed) {
        if (speed == EmuSpeed::Max) {
            m_soundEnabled = m_device->SoundIsEnabled();
            m_device->SoundEnable(false);
        }
        else
            m_device->SoundEnable(m_soundEnabled);
        m_speed = speed;
    }
}