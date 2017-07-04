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

        //��ȡ�ɶ�����ָ��
        const char* GetData();

        //��ȡ����������
        size_type GetBufferLen();

        //��ȡ�ɶ����ݳ���
        size_type GetReadableLen();

        //��ȡ��д��������С
        size_type GetWriteableLen();

        //��ȡ����д��������С
        size_type GetWriteableMax();

        //��ջ���������
        void Clear();

        //����ָ�����ȵĿɶ�����
        void Erase(size_type nEraseSize);

        //��������
        size_type Read(char* pData, size_type nSize);

        //д������
        size_type Write(const char* pData, size_type nSize);
    private:
        //�ƶ��ɶ����ݵ�������ͷ��
        void Align();

        //����������
        size_type DeAlloc(size_type nReqSize);

        //���仺����
        size_type ReAlloc(size_type nReqSize);

        //�����ڴ�
        size_type Alloc(size_type nAllocSize);

    private:
        CSockRecvBuffer(const CSockRecvBuffer&);
        CSockRecvBuffer operator =(const CSockRecvBuffer&);

    private:
        //������ָ��
        char* m_pHeadPointer;

        //��ָ��
        char* m_pReadPointer;

        //дָ��
        char* m_pWritePointer;

        //��������ǰ�ߴ�
        size_type m_nCurSize;

        //��������С�ߴ�
        size_type m_nMinSize;

        //���������ߴ�
        size_type m_nMaxSize;

        //
        static CBufferAllocator DefaultAllocator;
        CBufferAllocator* m_pRuntimeAlloctor;
    };
}

