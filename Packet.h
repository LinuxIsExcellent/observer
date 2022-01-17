#pragma once
#include <string.h>

#define MIN(a, b) a > b? b : a

class Packet
{
public:
    Packet()
    {
        m_pMemory = m_pMemoryEnd = m_pOffset = m_pDataEnd = NULL;
    }

    /*用一串内存来初始化包
     * lpDataPtr    内存的起始地址
     * dwSize       内存的大小
     * */
    Packet(const void* lpDataPtr, size_t dwSize)
    {
        m_pOffset = m_pMemory = (char*)lpDataPtr;
        m_pMemoryEnd = m_pDataEnd = m_pMemory + dwSize;
    }

    //重设包的偏移量
    void resetOffset()
    {
        m_pOffset = m_pMemory;
    }

    // 获得当前有效数据包的大小
    size_t getLength()
    {
        return m_pDataEnd - m_pMemory;
    }

    char* getDataBegin()
    {
        return m_pMemory;
    }
    //---------------------write function--------------------------------
    /*写入原子数据（原子数据包含字节数小于等于64字节的基本数据类型）
     * */
    template <typename T>
    void writeAtom(T data)
    {
        size_t dwMemSize = m_pMemoryEnd - m_pOffset;
        if (dwMemSize < sizeof(T))
        {
            setSize(m_pMemoryEnd - m_pMemory + sizeof(data));
        }

        *(T*)m_pOffset = data;
        m_pOffset += sizeof(data);

        if (m_pOffset > m_pDataEnd)
            m_pDataEnd = m_pOffset;
    }

    /*写入字符串
     * str      字符串指针
     * len      字符串长度
     * */

    void writeString(const char* str, size_t len = -1)
    {
        rawWriteStringPtr<unsigned short>(str, len);
    }

    /*写入字符串数据
     * 字符串的数据格式为2字节长度的字符串，总大小为65536字节,最后以为保留为终止字符，所以最大写入字符串长度为65535
     * TL 写入数据长度的数据类型，如果是短字符，就用unsigned char否则就用unsigned short
     * len 如果值为-1 则自动用strlen函数计算字符串的长度*/
    template<typename TL>
    void rawWriteStringPtr(const char* str, size_t len)
    {
        if (len == -1)
            len = str ? (TL)strlen(str) : 0;
        writeAtom<TL>((TL)len);
        writeBuf(str, len * sizeof(*str));
        writeAtom<char>(0);
    }

    /*写入一段内存数据
     * lpBuffer     内存的起始地址
     * dwSize       要写入的内存段的大小
     * */
    void writeBuf(const void* lpBuffer, size_t dwSize)
    {
        size_t dwMemSize = m_pMemoryEnd - m_pMemory;
        size_t offset = m_pOffset - m_pMemory;

        if (dwMemSize < dwSize)
        {
            setSize(m_pMemoryEnd - m_pMemory + dwSize);
        }

        memcpy(m_pOffset, lpBuffer, dwSize);
        m_pOffset += dwSize;

        if (m_pOffset > m_pDataEnd)
        {
            m_pDataEnd = m_pOffset;
        }

    }

    /*重新设置包的大小
     * dwNewSize        新的大小
     * */
    void setSize(size_t dwNewSize)
    {
        char* pOldMemory = m_pMemory;

        dwNewSize = (dwNewSize + MemoryAlignmentSize) & (~(MemoryAlignmentSize - 1));

        char* pMemory = (char*)malloc(dwNewSize);

        size_t dwOffset = m_pOffset - m_pMemory;
        size_t dwLength = m_pDataEnd - m_pMemory;

        if (dwLength > 0)
        {
            memcpy(pMemory, m_pMemory, MIN(dwNewSize, dwLength));
        }

        m_pMemory = pMemory;
        m_pMemoryEnd = m_pMemory + dwNewSize;

        m_pOffset = m_pMemory + dwOffset;
        m_pDataEnd = m_pMemory + dwLength;

        if (pOldMemory)
        {
            free(pOldMemory);
        }
    }

    void print_info()
    {
        printf("指针 m_pMemory 的地址为：%p\n", m_pMemory);
        printf("指针 m_pMemoryEnd 的地址为：%p\n", m_pMemoryEnd);
        printf("指针 m_pOffset 的地址为：%p\n", m_pOffset);
        printf("指针 m_pDataEnd 的地址为：%p\n", m_pDataEnd);
    }


    template <typename T>
    inline Packet& operator << (T val)
    {
        if (sizeof(T) < sizeof (long long))
            writeAtom<T>(val);
        else writeBuf(&val, sizeof(val));

        return *this;
    }

    Packet& operator << (const char* val)
    {
        writeString(val, strlen(val));
        return *this;
    }


    Packet& operator << (char* val)
    {
        writeString(val, strlen(val));
        return *this;
    }
    //---------------------write function--------------------------------

    //---------------------read function--------------------------------
    size_t readBuf(void* lpBuffer, size_t dwSize)
    {
        size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
        if (dwSize > dwAvaliableSize)
            dwSize = dwAvaliableSize;
        if (dwSize > 0)
        {
            memcpy(lpBuffer, m_pOffset, dwSize);
            m_pOffset += dwSize;
        }

        return dwSize;

    }

    template <typename T>
    T readAtom()
    {
        T val = T();
        long long dwAvaliableSize = m_pDataEnd - m_pOffset;
        if (dwAvaliableSize < 0)
        {
            return 0;
        }

        if (dwAvaliableSize >= sizeof (T))
        {
            val = *(T*)m_pOffset;
            m_pOffset += sizeof(T);
        }
        else if (dwAvaliableSize)
        {
            memset(&val, 0, sizeof(val));
            memcpy(&val, m_pOffset, dwAvaliableSize);
            m_pOffset += dwAvaliableSize;
        }

        return val;
    }

    template <typename TS, typename TL>
    const TS* rawReadStringPtr()
    {
        size_t dwAvaliableSize = m_pDataEnd - m_pOffset;
        size_t nStrLen = 0;

        if (dwAvaliableSize >= sizeof (TL) + sizeof(TS))
        {
            nStrLen = *((TL*)m_pOffset);

            if (dwAvaliableSize >= nStrLen + sizeof(TL) + sizeof(TS))
            {
                const TS* str = (TS*)(m_pOffset + sizeof(TL));
                m_pOffset += nStrLen *  sizeof(TS) + sizeof(TL) + sizeof(TS);
                return str;
            }
        }

        return NULL;
    }

    template <typename T>
    inline Packet& operator >> (T& val)
    {
        if (sizeof(val) <= sizeof (long long))
            val = readAtom<T>();
        else readBuf(&val, sizeof(val));

        return *this;
    }

    inline Packet& operator >> (const char* &str)
    {
        str = rawReadStringPtr<char, unsigned short>();
        return *this;
    }
    //---------------------read function--------------------------------
private:
    char* m_pMemory;
    char* m_pMemoryEnd;
    char* m_pOffset;
    char* m_pDataEnd;

    static const size_t MemoryAlignmentSize = 256;  //数据包内存长度对齐边界
};
