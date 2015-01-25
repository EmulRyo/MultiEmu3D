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

#ifndef __VIDEOGAMEDEVICE_H__
#define __VIDEOGAMEDEVICE_H__

class IScreenDrawable;

enum e_devicetype { GAMEBOY, GAMEBOYCOLOR, MASTERSYSTEM, GAMEGEAR };
    
class VideoGameDevice {
public:
    virtual ~VideoGameDevice() { };
    
    virtual void Reset() = 0;
    virtual void ExecuteOneFrame() = 0;
    virtual void CartridgeExtract() = 0;
    virtual void CartridgeLoad(const std::string &fileName, const std::string &batteriesPath="",
                               unsigned char *cartridgeBuffer=NULL, unsigned long size=0) = 0;
    virtual bool SoundIsEnabled() = 0;
    virtual void SoundEnable(bool value) = 0;
    virtual void SoundSetSampleRate(long sampleRate) = 0;
    virtual void LoadState(const std::string &fileName, int id) = 0;
    virtual void SaveState(const std::string &fileName, int id) = 0;
    virtual void LoadStateFromRAM(std::istream *stream) = 0;
    virtual void SaveStateToRAM(std::ostream *stream) = 0;
    virtual void PadSetButtons(bool *buttonsState) = 0;
    virtual int  PadIdAcceptButton() = 0;
    virtual int  PadIdCancelButton() = 0;
    virtual int  PadIdLeftButton() = 0;
    virtual int  PadIdRightButton() = 0;
    virtual void SetScreen(IScreenDrawable *screen) = 0;
    virtual int  GetWidth() = 0;
    virtual int  GetHeight() = 0;
    virtual void SetExtraData(const std::string &key, void *value) = 0;
    
    e_devicetype GetType() { return m_deviceType; };
    void SetType(e_devicetype type) { m_deviceType = type; };
    
private:
    e_devicetype m_deviceType;
};

#endif
