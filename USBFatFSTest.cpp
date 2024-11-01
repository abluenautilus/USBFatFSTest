#include "daisy_patch_sm.h"
#include "daisy_patchsm_usb.h"
#include "fatfs.h"

using namespace daisy;
using namespace patch_sm;

DaisyPatchSM_USB   hw;
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

/* USB Host callbacks */
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
void USBErrorCallback(void* userdata) {
    hw.PrintLine("USB Error");
}

int main(void)
{
    hw.Init();
    hw.StartLog(true);
    hw.PrintLine("Daisy Patch SM started. Test Beginning");

    /* SET UP USB*/
    const char* usbPath = hw.fatfs_interface.GetUSBPath();
    snprintf(testFilePath, IO_BUFFER_SIZE, "%s%s", usbPath, TEST_FILE_NAME);
    hw.PrepareMedia(USBConnectCallback, USBDisconnectCallback, USBClassActiveCallback, USBErrorCallback);

    /** Strings for write/read to text file */
    const char *test_string = "Testing Daisy Patch SM";
    const char *test_fname  = "DaisyPatchSM-USBTest.txt";

    uint32_t now;
    now = System::GetNow();

    while(1)
    {
        now = System::GetNow();

        hw.usbHost.Process();

        if ((now%5000)==0) {

            FRESULT fres = FR_DENIED; 
         
            /** Write Test */
            fres = f_open(&file, testFilePath, (FA_CREATE_ALWAYS | FA_WRITE));
            if(fres == FR_OK)
            {
                UINT   bw  = 0;
                size_t len = strlen(test_string);
                fres       = f_write(&file, test_string, len, &bw);
            } else {
                hw.PrintLine("Write test f_open failed with code: %d",fres);
            }
            f_close(&file);
            if(fres == FR_OK)
            {
                /** Read Test only if Write passed */
                fres = f_open(&file, test_fname, (FA_OPEN_EXISTING | FA_READ));
                if(fres == FR_OK)
                {
                    UINT   br = 0;
                    char   readbuff[32];
                    size_t len = strlen(test_string);
                    fres       = f_read(&file, readbuff, len, &br);
                } else {
                    hw.PrintLine("Read test f_open failed with code: %d",fres);
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
