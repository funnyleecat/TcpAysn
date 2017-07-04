#pragma once

#include <cstddef>

namespace tcpcomm2
{
#pragma once
#include <cstddef>
    class CBufferAllocator
    {
    public:
        typedef std::size_t size_type;

        CBufferAllocator(){}
        virtual ~CBufferAllocator(){}
        virtual char* Alloc(size_type nSize){ return new(std::nothrow) char[nSize]; }
        virtual void DeAlloc(char* pData){ delete[] pData; }

    private:
        CBufferAllocator(const CBufferAllocator&);
        CBufferAllocator operator =(const CBufferAllocator&);
    };

    class CSockRecvBuffer
    {
    public:
        typedef std::size_t size_type;
        CSockRecvBuffer(size_type nMin, size_type nMax);
        virtual ~CSockRecvBuffer();

        //获取可读数据指针
        const char* GetData();

        //获取缓冲区长度
        size_type GetBufferLen();

        //获取可读数据长度
        size_type GetReadableLen();

        //获取可写缓冲区大小
        size_type GetWriteableLen();

        //获取最大可写缓冲区大小
        size_type GetWriteableMax();

        //清空缓冲区数据
        void Clear();

        //擦除指定长度的可读数据
        void Erase(size_type nEraseSize);

        //读出数据
        size_type Read(char* pData, size_type nSize);

        //写入数据
        size_type Write(const char* pData, size_type nSize);
    private:
        //移动可读数据到缓冲区头部
        void Align();

        //收缩缓冲区
        size_type DeAlloc(size_type nReqSize);

        //扩充缓冲区
        size_type ReAlloc(size_type nReqSize);

        //申请内存
        size_type Alloc(size_type nAllocSize);

    private:
        CSockRecvBuffer(const CSockRecvBuffer&);
        CSockRecvBuffer operator =(const CSockRecvBuffer&);

    private:
        //缓冲区指针
        char* m_pHeadPointer;

        //读指针
        char* m_pReadPointer;

        //写指针
        char* m_pWritePointer;

        //缓冲区当前尺寸
        size_type m_nCurSize;

        //缓冲区最小尺寸
        size_type m_nMinSize;

        //缓冲区最大尺寸
        size_type m_nMaxSize;

        //
        static CBufferAllocator DefaultAllocator;
        CBufferAllocator* m_pRuntimeAlloctor;
    };
}

