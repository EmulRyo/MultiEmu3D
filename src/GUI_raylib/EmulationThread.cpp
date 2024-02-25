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

#include <thread>
#include <string>
#include <chrono>
#include <filesystem>
#include "../SMS-GG/SMS.h"
#include "../GB-GBC/GB.h"
#include "../NES/NES.h"
#include "../Common/VideoGameDevice.h"
#include "../Common/Exception.h"
#include "../Common/Utils.h"
//#include "Settings.h"
#include "RendererBase.h"
#include "EmulationThread.h"

#include "raylib.h"
#define RAYLIB_PHYSFS_IMPLEMENTATION
#include "raylib-physfs.h"
//#include "Joystick.h"
//#include "RendererOGL.h"
//#include "Rewind.h"


EmulationThread::EmulationThread()
{
    m_screen = NULL;
    m_device = NULL;
    m_fps = 0;
    m_threadShouldClose = false;
    //m_rewind = NULL;
    
    m_emuState = EmuState::NotStartedYet;
    
    //joystick = new Joystick();
    m_finished = false;
    m_speed = EmuSpeed::Normal;
    for (int i = 0; i < 16; i++)
        keysUsed[i] = 0;
    m_soundEnabled = false;

    m_thread = new std::thread(&EmulationThread::Entry, this);
}

EmulationThread::~EmulationThread() {
    m_emuState = EmuState::Stopped;
    //delete m_rewind;
    if (m_device)
        delete m_device;
    
    //delete joystick;
}

void EmulationThread::SetState(EmuState state)
{
    if (!m_device)
        return;

    std::lock_guard<std::mutex> lg(m_mutex);
    
    m_emuState = state;
    
    if (state == EmuState::Playing) {
        //m_device->SoundEnable(SettingsGetSoundEnabled());
        ((RendererBase *)m_screen)->SetIcon(Renderer::Play);
    }
    else
        m_device->SoundEnable(false);
    
    if (state == EmuState::Stopped)
    {
        ((RendererBase *)m_screen)->SetIcon(Renderer::Stop);
        ((RendererBase *)m_screen)->SetRewindValue(-1);
        //m_rewind->Disable();
        
        m_device->CartridgeExtract();
        m_device->Reset();
    }
    else if (state == EmuState::Paused) {
        ((RendererBase *)m_screen)->SetIcon(Renderer::Pause);
    }
}

EmuState EmulationThread::GetState()
{
    return m_emuState;
}

long long EmulationThread::ElapsedMicroSeconds(std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

long long EmulationThread::ElapsedMilliSeconds(std::chrono::steady_clock::time_point begin, std::chrono::steady_clock::time_point end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

void EmulationThread::Entry()
{
    const long long desired = 1000000/60;  // Microsegundos deseados por frame
    long long accumulated = 0;
    int frames = 0;
    auto fpsBegin = std::chrono::steady_clock::now();
    auto fpsEnd = fpsBegin;
    try {
        while (!m_threadShouldClose)
        {
            auto frameBegin = std::chrono::steady_clock::now();
            {
                std::lock_guard<std::mutex> lg(m_mutex);
                if (m_emuState == EmuState::Playing)
                {
                    //if (!m_rewind->IsEnabled()) {
                    m_device->ExecuteOneFrame();
                    //m_rewind->AddFrame();
                    frames++;
                    //}
                }
            }   // Desbloquear el mutex

            auto frameEnd = std::chrono::steady_clock::now();
            if (m_speed == EmuSpeed::Normal) {
                while ((ElapsedMicroSeconds(frameBegin, frameEnd) + accumulated) < desired) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    frameEnd = std::chrono::steady_clock::now();
                }
                accumulated = ElapsedMicroSeconds(frameBegin, frameEnd) + accumulated - desired;
            }
            
            fpsEnd = std::chrono::steady_clock::now();
            long long elapsed = ElapsedMilliSeconds(fpsBegin, fpsEnd);
            if (elapsed > 1000) {
                m_fps = frames * 1000.0f / elapsed;
                PrintfVisualStudioOutput("FPS: %f\n", m_fps);
                fpsBegin = fpsEnd;
                frames = 0;
            }
        }
    }
    catch (Exception& exc) {
        //wxMessageBox(exc.what());
        printf("%s\n", exc.what());
    }
    
    m_finished = true;
}

void EmulationThread::Exit() {
    m_threadShouldClose = true;
    m_thread->join();
}

float EmulationThread::GetFPS() {
    if (m_emuState == EmuState::Playing)
        return m_fps;
    else
        return 0;
}

bool EmulationThread::ChangeFile(const std::string& fileName)
{
    // Meter dentro de las llaves para que se desbloquee el mutex antes de
    // llamar a SetState
    {
        std::lock_guard<std::mutex> lg(m_mutex);

        if (!std::filesystem::exists(fileName)) {
            printf("The file: %s doesn't exist", fileName.c_str());
            return false;
        }

        size_t dotPos = fileName.rfind('.');
        if (dotPos == std::string::npos) {
            printf("File without extension");
            return false;
        }

        std::string extension = fileName.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        u8* buffer = nullptr;
        unsigned long size = 0;
        bool zip, sms, gb, nes;
        zip = sms = gb = nes = false;

        if (extension == "zip") {
            zip = true;
            LoadZip(fileName, &buffer, &size, extension);
            if ((buffer == nullptr) || (size == 0))
                return false;
        }

        sms = MasterSystem::SMS::IsValidExtension(extension);
        gb = GameBoy::GB::IsValidExtension(extension);
        nes = Nes::NES::IsValidExtension(extension);
        if (!sms && !gb && !nes) {
            printf("Not valid files found!");
            return false;
        }

        // Si ha llegado aquí es que es un archivo permitido
        std::filesystem::path exeDir = std::filesystem::absolute(std::filesystem::path("."));
        std::filesystem::path battsDir = exeDir.append("Batts");
        if (!std::filesystem::exists(battsDir))
            std::filesystem::create_directory(battsDir);

        battsDir += battsDir.preferred_separator;

        if (m_device)
            delete m_device;

        if (sms)
            m_device = new MasterSystem::SMS();
        else if (gb)
            m_device = new GameBoy::GB();
        else if (nes)
            m_device = new Nes::NES();

        //m_rewind = new Rewind(m_device);
        //ApplySettingsNoMutex();
        SetScreenNoMutex(m_screen);

        if (zip)
            m_device->CartridgeLoad(fileName, battsDir.string(), buffer, size);
        else
            m_device->CartridgeLoad(fileName, battsDir.string());
    }
    
	SetState(EmuState::Playing);
    
    return true;
}

/*
 * Carga un fichero comprimido con zip y busca una rom.
 * Si existe mas de una rom solo carga la primera. Si se ha encontrado, la rom se devuelve en un buffer
 * junto con su tamaño, sino las variables se dejan intactas
 */
void EmulationThread::LoadZip(const std::string &zipPath, u8 **buffer, unsigned long *size, std::string &extension)
{
    bool success = InitPhysFSEx(zipPath.c_str(), "content");
    if (!success)
        return;

    FilePathList filePathList = LoadDirectoryFilesFromPhysFS("content");
    for (unsigned int i = 0; i < filePathList.count; i++) {
        extension = std::string(GetFileExtension(filePathList.paths[i]) + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c) { return std::tolower(c); });

        bool sms = MasterSystem::SMS::IsValidExtension(extension);
        bool gb = GameBoy::GB::IsValidExtension(extension);
        bool nes = Nes::NES::IsValidExtension(extension);
        if (sms || gb || nes)
        {
            int bytesRead = 0;
            const char* fileName = TextFormat("content/%s", filePathList.paths[i]);
            unsigned char* fileData = LoadFileDataFromPhysFS(fileName, &bytesRead);
            if (bytesRead > 0) {
                *size = (unsigned long)bytesRead;
                *buffer = new u8[*size];
                memcpy(*buffer, fileData, (size_t)bytesRead);
                UnloadFileData(fileData);
                ClosePhysFS();
                UnloadDirectoryFiles(filePathList);
                return;
            }
        }
    }

    ClosePhysFS();
    UnloadDirectoryFiles(filePathList);
    
	// Archivo no encontrado
	printf("Not valid rom found in %s\n", zipPath.c_str());
}

void EmulationThread::LoadState(const std::string &fileName, int id)
{
    m_mutex.lock();
    m_device->LoadState(fileName, id);
    m_mutex.unlock();
}

void EmulationThread::SaveState(const std::string &fileName, int id)
{
    m_mutex.lock();
    m_device->SaveState(fileName, id);
    m_mutex.unlock();
}

void EmulationThread::ApplySettings()
{
    m_mutex.lock();
    ApplySettingsNoMutex();
    m_mutex.unlock();
}

void EmulationThread::ApplySettingsNoMutex()
{
    if (m_device) {
        /*
        PadSetKeys(SettingsGetInput(m_device->GetType()));
        m_device->SoundSetSampleRate(SettingsGetSoundSampleRate());
        m_device->SoundEnable(SettingsGetSoundEnabled());
        */
    }
}

void EmulationThread::SetScreen(IScreenDrawable *screen) {
    m_mutex.lock();
    SetScreenNoMutex(screen);
    m_mutex.unlock();
}

void EmulationThread::SetScreenNoMutex(IScreenDrawable *screen) {
    m_screen = screen;
    if (m_device)
        m_device->SetScreen(screen);
    /*
    if (m_rewind)
        m_rewind->SetRenderer((RendererBase *)screen);
    */
}

void EmulationThread::UpdatePad()
{
    if (m_emuState == EmuState::Playing) {
        /*
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
        */
    }
}

VideoGameDevice *EmulationThread::GetVideoGameDevice() {
    return m_device;
}

void EmulationThread::PadSetKeys(int* keys) {
    /*
    for (int i=0; i<m_device->PadGetNumButtons(); i++)
		keysUsed[i] = (wxKeyCode)keys[i];
    */
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