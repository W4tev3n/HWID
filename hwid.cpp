/*
MIT License

Copyright (c) 2022 W4tev3n

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define HWID_DIRECTX_SDK

#include <Windows.h>
#include <Atlbase.h>
#include <Wbemidl.h> 
#include <Iphlpapi.h>

#include <comdef.h>
#include <intrin.h>

#include <string>
#include <sstream>

#ifdef HWID_DIRECTX_SDK
#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")
#endif

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")

namespace HWID
{
    static const uint32_t CRC32_Table[] =
    {
        0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL, 0x076DC419L, 0x706AF48FL, 0xE963A535L, 0x9E6495A3L,
        0x0EDB8832L, 0x79DCB8A4L, 0xE0D5E91EL, 0x97D2D988L, 0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L, 0x90BF1D91L,
        0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL, 0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L,
        0x136C9856L, 0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL, 0x14015C4FL, 0x63066CD9L, 0xFA0F3D63L, 0x8D080DF5L,
        0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L, 0xA2677172L, 0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
        0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L, 0x32D86CE3L, 0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L,
        0x26D930ACL, 0x51DE003AL, 0xC8D75180L, 0xBFD06116L, 0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L, 0xB8BDA50FL,
        0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L, 0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL,
        0x76DC4190L, 0x01DB7106L, 0x98D220BCL, 0xEFD5102AL, 0x71B18589L, 0x06B6B51FL, 0x9FBFE4A5L, 0xE8B8D433L,
        0x7807C9A2L, 0x0F00F934L, 0x9609A88EL, 0xE10E9818L, 0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
        0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL, 0x6C0695EDL, 0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L,
        0x65B0D9C6L, 0x12B7E950L, 0x8BBEB8EAL, 0xFCB9887CL, 0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L, 0xFBD44C65L,
        0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L, 0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL,
        0x4369E96AL, 0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L, 0x44042D73L, 0x33031DE5L, 0xAA0A4C5FL, 0xDD0D7CC9L,
        0x5005713CL, 0x270241AAL, 0xBE0B1010L, 0xC90C2086L, 0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
        0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L, 0x59B33D17L, 0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL,
        0xEDB88320L, 0x9ABFB3B6L, 0x03B6E20CL, 0x74B1D29AL, 0xEAD54739L, 0x9DD277AFL, 0x04DB2615L, 0x73DC1683L,
        0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L, 0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L,
        0xF00F9344L, 0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL, 0xF762575DL, 0x806567CBL, 0x196C3671L, 0x6E6B06E7L,
        0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL, 0x67DD4ACCL, 0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
        0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L, 0xD1BB67F1L, 0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL,
        0xD80D2BDAL, 0xAF0A1B4CL, 0x36034AF6L, 0x41047A60L, 0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL, 0x4669BE79L,
        0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L, 0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL,
        0xC5BA3BBEL, 0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L, 0xC2D7FFA7L, 0xB5D0CF31L, 0x2CD99E8BL, 0x5BDEAE1DL,
        0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL, 0x026D930AL, 0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
        0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L, 0x92D28E9BL, 0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L,
        0x86D3D2D4L, 0xF1D4E242L, 0x68DDB3F8L, 0x1FDA836EL, 0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L, 0x18B74777L,
        0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL, 0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L,
        0xA00AE278L, 0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L, 0xA7672661L, 0xD06016F7L, 0x4969474DL, 0x3E6E77DBL,
        0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L, 0x37D83BF0L, 0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
        0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L, 0xBAD03605L, 0xCDD70693L, 0x54DE5729L, 0x23D967BFL,
        0xB3667A2EL, 0xC4614AB8L, 0x5D681B02L, 0x2A6F2B94L, 0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL, 0x2D02EF8DL
    };

    // Hashing algorithm.
    uint32_t CRC32(uint8_t* Data, size_t Size) {
        uint32_t Hash = 0xFFFFFFFF;
        for (uint32_t I = 0; I < Size; ++I)
            Hash = CRC32_Table[(Hash ^ Data[I]) & 0xFF] ^ (Hash >> 8);
        return Hash ^ 0xFFFFFFFF;
    };

    // Fast and simple string hashing algorithm.
    uint32_t ElfHash(uint8_t* s) 
    {
        uint32_t Hash = 0, High = 0;
        while (*s) 
        {
            Hash = (Hash << 4) + *s++;
            if (High = Hash & 0xF0000000)
                Hash ^= High >> 24;
            Hash &= ~High;
        }
        return Hash;
    }

    // Emulating getenv with getenv_s, since its deprecated.
    const size_t EnvBufSize = 512;
    size_t EnvSize = 512;
    char EnvResult[EnvBufSize];
    
    const char* GetEnv(const char* VarName) {
        memset(EnvResult, '\x00', EnvSize);
        EnvSize = EnvBufSize;
        getenv_s(&EnvSize, EnvResult, VarName);
        return EnvResult;
    }

    // Request a hash of the last property from WMI.
    uint32_t WMIProperyHash(LPCWSTR Server, LPCWSTR Request, LPCWSTR Property)
    {
        HRESULT hres;

        hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres)) 
        {
            return 0x00;
        }

        hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

        if (FAILED(hres)) 
        {
            CoUninitialize();
            return 0x00;
        }

        IWbemLocator* pLoc = NULL;

        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

        if (FAILED(hres)) 
        {
            CoUninitialize();
            return 0x00;
        }

        IWbemServices* pSvc = NULL;

        hres = pLoc->ConnectServer(_bstr_t(Server), NULL, NULL, 0, NULL, 0, 0, &pSvc);

        if (FAILED(hres)) 
        {
            pLoc->Release();
            CoUninitialize();
            return 0x00;
        }

        hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

        if (FAILED(hres)) 
        {
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return 0x00;
        }

        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t(Request), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);

        if (FAILED(hres)) 
        {
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            return 0x00;
        }

        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;

        DWORD64 Result = NULL;

        while (pEnumerator) 
        {
            HRESULT hRes = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

            if (0 == uReturn) 
            {
                break;
            }

            VARIANT vtProp = {};

            hRes = pclsObj->Get(Property, 0, &vtProp, 0, 0);
            Result ^= CRC32((byte*)vtProp.bstrVal, lstrlenW(vtProp.bstrVal));
            VariantClear(&vtProp);

            pclsObj->Release();
        }

        pSvc->Release();
        pLoc->Release();
        pEnumerator->Release();
        CoUninitialize();

        return Result;
    };

    // Wrapper around cpuid intrinsic.
    class CpuId 
    {
        uint32_t regs[4];

    public:
        explicit CpuId(unsigned i) 
        {
            __cpuid((int*)regs, (int)i);
        }

        const uint32_t& EAX() const { return regs[0]; }
        const uint32_t& EBX() const { return regs[1]; }
        const uint32_t& ECX() const { return regs[2]; }
        const uint32_t& EDX() const { return regs[3]; }
    };

    // Request processor manufacturer and features hash.
    uint32_t CPU_Id()
    {
        INT Data[8] = { 0 };

        // Highest Function Parameter and Manufacturer ID.
        CpuId Vendor = CpuId(0);
        Data[0] = Vendor.EAX();
        Data[1] = Vendor.EBX();
        Data[2] = Vendor.ECX();
        Data[3] = Vendor.EDX();

        // Feature Bits.
        CpuId Features = CpuId(1);
        Data[4] = Features.EDX();
        Data[5] = Features.ECX();

        return CRC32((BYTE*)Data, sizeof(Data));
    };

    // Request physical drives serial numbers hash.
    uint32_t Disk_Id()
    {
        auto HDDID_For = [&](int i) -> int {
            std::string DeviceName = "\\\\.\\PhysicalDrive";
            DeviceName += std::to_string(i);
            HANDLE h = CreateFileA(DeviceName.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            if (h == INVALID_HANDLE_VALUE) return {};
            std::unique_ptr<std::remove_pointer<HANDLE>::type, void(*)(HANDLE)> hDevice{ h, [](HANDLE handle) {CloseHandle(handle); } };
            STORAGE_PROPERTY_QUERY storagePropertyQuery{};
            storagePropertyQuery.PropertyId = StorageDeviceProperty;
            storagePropertyQuery.QueryType = PropertyStandardQuery;
            STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader{};
            DWORD dwBytesReturned = 0;
            if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
                &storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &dwBytesReturned, NULL))
                return {};
            const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
            std::unique_ptr<BYTE[]> pOutBuffer{ new BYTE[dwOutBufferSize]{} };
            if (!DeviceIoControl(hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
                pOutBuffer.get(), dwOutBufferSize, &dwBytesReturned, NULL))
                return {};
            STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(pOutBuffer.get());
            const DWORD dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
            if (dwSerialNumberOffset == 0) return {};
            const char* serialNumber = reinterpret_cast<const char*>(pOutBuffer.get() + dwSerialNumberOffset);
            return ElfHash((uint8_t*)serialNumber);
        };
        uint32_t Hash = 0;
        for (int i = 0; i < 16; i++)
            Hash ^= HDDID_For(i);
        return Hash;
    };

    // Request video adapter descriptions hash.
#ifdef HWID_DIRECTX_SDK
    uint32_t Video_Id()
    {
        IDirect3D9* D3D9Object = Direct3DCreate9(D3D_SDK_VERSION);
        D3DPRESENT_PARAMETERS D3D9Present = { 0 };
        D3D9Present.Windowed = TRUE;
        D3D9Present.SwapEffect = D3DSWAPEFFECT_DISCARD;
        UINT AdapterCount = D3D9Object->GetAdapterCount();
        D3DADAPTER_IDENTIFIER9* Adapters = new D3DADAPTER_IDENTIFIER9[AdapterCount];

        for (DWORD I = 0; I < AdapterCount; I++)
            D3D9Object->GetAdapterIdentifier(I, 0, &(Adapters[I]));

        return ElfHash((uint8_t*)Adapters->Description);
    }
#else
    uint32_t Video_Id()
    {
        return NULL;
    }
#endif

    // Request network adapters mac addresses hash.
    uint32_t Adapter_Id()
    {
        IP_ADAPTER_INFO AdapterInfo[16];
        DWORD dwBufLen = sizeof(AdapterInfo);
        DWORD dwMacId = 0;

        DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

        if (dwStatus != ERROR_SUCCESS)
            return -1;

        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        do
        {
            dwMacId ^= CRC32((BYTE*)pAdapterInfo->Address, 8);
            pAdapterInfo = pAdapterInfo->Next;
        } while (pAdapterInfo);

        return dwMacId;
    }

    // Request Windows GUID and profile name hash.
    uint32_t OS_Id()
    {
        HW_PROFILE_INFOA hwProfileInfo;
        std::string ProfUID{};
        uint32_t Hash{};

        if (GetCurrentHwProfileA(&hwProfileInfo) != NULL) 
        {
            ProfUID += hwProfileInfo.szHwProfileGuid;
            ProfUID += hwProfileInfo.szHwProfileName;

            Hash = CRC32((byte*)ProfUID.data(), ProfUID.size());
        }
        else 
        {
            Hash = 0;
        }
        return Hash;
    };

    // Request processor identifiers from the environment.
    uint32_t CPU_Env()
    {
        uint32_t Hash{};
        Hash ^= ElfHash((uint8_t*)GetEnv("PROCESSOR_ARCHITECTURE"));
        Hash ^= ElfHash((uint8_t*)GetEnv("PROCESSOR_IDENTIFIER"));
        Hash ^= ElfHash((uint8_t*)GetEnv("PROCESSOR_LEVEL"));
        Hash ^= ElfHash((uint8_t*)GetEnv("PROCESSOR_REVISION"));
        return Hash;
    }

    // Generate INI string, with current hardware fingerprint.
    std::string Fingerprint() 
    {
        std::stringstream sstream{};

        sstream << "[Hardware]" << std::endl;
        sstream << "OS_Name = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_OperatingSystem", L"Name") << std::endl;
        sstream << "OS_Serial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_OperatingSystem", L"SerialNumber") << std::endl;
        sstream << "OS_Id = " << OS_Id() << std::endl;
        sstream << "OS_Username = " << GetEnv("USERNAME") << std::endl;

        sstream << "CPU_Name = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_Processor", L"Caption") << std::endl;
        sstream << "CPU_Serial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_Processor", L"SerialNumber") << std::endl;
        sstream << "CPU_Id = " << CPU_Id() << std::endl;
        sstream << "CPU_Env = " << CPU_Env() << std::endl;

        sstream << "BIOS_Name = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_BIOS", L"Caption") << std::endl;
        sstream << "BIOS_Manufacturer = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_BIOS", L"Manufacturer") << std::endl;
        sstream << "BIOS_Serial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_BIOS", L"SerialNumber") << std::endl;

        sstream << "Board_Name = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_BaseBoard", L"Product") << std::endl;
        sstream << "Board_Serial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_BaseBoard", L"SerialNumber") << std::endl;

        sstream << "Disk_VolumeSerial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_LogicalDisk", L"VolumeSerialNumber") << std::endl;
        sstream << "Disk_Serial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_DiskDrive", L"SerialNumber") << std::endl;
        sstream << "Disk_Id = " << Disk_Id() << std::endl;

        sstream << "Adapter_Name = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_NetworkAdapter", L"ProductName") << std::endl;
        sstream << "Adapter_Id = " << Adapter_Id() << std::endl;

        sstream << "Video_Name = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_VideoController", L"Caption") << std::endl;
        sstream << "Video_Serial = " << WMIProperyHash(L"ROOT\\CIMV2", L"SELECT * FROM Win32_VideoController", L"PNPDeviceID") << std::endl;
        sstream << "Video_Id = " << Video_Id() << std::endl;

        return sstream.str();
    }
}