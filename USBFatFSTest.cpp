#include "daisy_patch_sm.h"
#include "daisysp.h"
#include "fatfs.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

DaisyPatchSM   hw;
FIL            file; /**< Can't be made on the stack (DTCMRAM) */
FatFSInterface fsi;
FRESULT fileResult;

#define IO_BUFFER_SIZE 100
#define TEST_FILE_NAME "USB_Test.txt"
char testFilePath[IO_BUFFER_SIZE];

bool isUsbConnected = false;
bool isUsbConfigured = false;
bool wasConfigLoadAttempted = false;
bool isConfigChanged = false;

daisy::USBHostHandle usb;
daisy::FatFSInterface fatfs_interface;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    hw.ProcessAllControls();
    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
    }
}


void USBConnectCallback(void* userdata) {
	isUsbConnected = true;
    hw.PrintLine("USB Connected");
}

void USBDisconnectCallback(void* userdata) {
	isUsbConnected = false;
	isUsbConfigured = false;
    hw.PrintLine("USB Disconnected");
}

void USBClassActiveCallback(void* userdata) {
	isUsbConfigured = true;
}

/** @brief starts the mounting process for USB Drive use if it is present
 *  --FROM AURORA SDK--
 */
void PrepareMedia(
    daisy::USBHostHandle::ConnectCallback     connect_cb      = nullptr,
    daisy::USBHostHandle::DisconnectCallback  disconnect_cb   = nullptr,
    daisy::USBHostHandle::ClassActiveCallback class_active_cb = nullptr,
    daisy::USBHostHandle::ErrorCallback       error_cb        = nullptr,
    void                                     *userdata        = nullptr)
{
    /** Initialize hardware and set user callbacks */
    daisy::USBHostHandle::Config usbcfg;
    usbcfg.connect_callback      = connect_cb;
    usbcfg.disconnect_callback   = disconnect_cb;
    usbcfg.class_active_callback = class_active_cb;
    usbcfg.error_callback        = error_cb;
    usbcfg.userdata              = userdata;
    usb.Init(usbcfg);

    /** Prepare FatFS -- fmount will defer until first attempt to read/write */
    daisy::FatFSInterface::Config fsi_cfg;
    fsi_cfg.media = daisy::FatFSInterface::Config::MEDIA_USB;
    fatfs_interface.Init(fsi_cfg);
    f_mount(&fatfs_interface.GetUSBFileSystem(),
            fatfs_interface.GetUSBPath(),
            0);
}

int main(void)
{
    hw.Init();
    hw.StartLog(true);
    hw.PrintLine("Daisy Patch SM started. Test Beginning");

    /* SET UP USB*/
    const char* usbPath = fatfs_interface.GetUSBPath();
    snprintf(testFilePath, IO_BUFFER_SIZE, "%s%s", usbPath, TEST_FILE_NAME);
    PrepareMedia(USBConnectCallback, USBDisconnectCallback, USBClassActiveCallback);

    /** Write/Read text file */
    const char *test_string = "Testing Daisy Patch SM";
    const char *test_fname  = "DaisyPatchSM-USBTest.txt";

    uint32_t now, dact, usbt, gatet;
    now = dact = usbt = System::GetNow();

    while(1)
    {
        now = System::GetNow();

        usb.Process();

        if ((now%5000)==0) {

            FRESULT fres = FR_DENIED; /**< Unlikely to actually experience this */
         
            /** Write Test */
            fres = f_open(&file, testFilePath, (FA_CREATE_ALWAYS | FA_WRITE));
            if(fres == FR_OK)
            {
                UINT   bw  = 0;
                size_t len = strlen(test_string);
                fres       = f_write(&file, test_string, len, &bw);
            } else {
                hw.PrintLine("F_open failed with code: %d",fres);
            }
            f_close(&file);
            if(fres == FR_OK)
            {
                /** Read Test only if Write passed */
                if(f_open(&file, test_fname, (FA_OPEN_EXISTING | FA_READ)) == FR_OK)
                {
                    UINT   br = 0;
                    char   readbuff[32];
                    size_t len = strlen(test_string);
                    fres       = f_read(&file, readbuff, len, &br);
                }
                f_close(&file);
            }
        
            bool usb_pass = fres == FR_OK;
            hw.PrintLine("USB Test\t-\t%s", usb_pass ? "PASS" : "FAIL");

            System::Delay(50);
        }

        /** short 60ms blip off on builtin LED */
        hw.SetLed((now & 2047) > 60);
    }
}
