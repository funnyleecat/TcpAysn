#include "stdafx.h"
#include "SockRecvBuffer.h"
#include <string>
#include <algorithm>
#include <assert.h>

namespace tcpcomm2
{
    CBufferAllocator CSockRecvBuffer::DefaultAllocator;

    CSockRecvBuffer::CSockRecvBuffer(size_type nMin, size_type nMax)
        : m_pRuntimeAlloctor(&DefaultAllocator)
        , m_nCurSize(nMin)
        , m_nMinSize(nMin)
        , m_nMaxSize(nMax)
    {
        m_pHeadPointer = m_pRuntimeAlloctor->Alloc(m_nCurSize);
        m_pReadPointer = m_pHeadPointer;
        m_pWritePointer = m_pHeadPointer;
    }

    CSockRecvBuffer::~CSockRecvBuffer()
    {
        if (m_pHeadPointer)
        {
            m_pRuntimeAlloctor->DeAlloc(m_pHeadPointer);
            m_pHeadPointer = NULL;
        }
    }

    const char* CSockRecvBuffer::GetData()
    {
        return m_pReadPointer;
    }

    CSockRecvBuffer::size_type CSockRecvBuffer::GetBufferLen()
    {
        return m_nCurSize;
    }

    CSockRecvBuffer::size_type CSockRecvBuffer::GetReadableLen()
    {
        return m_pWritePointer - m_pReadPointer;
    }

    CSockRecvBuffer::size_type CSockRecvBuffer::GetWriteableLen()
    {
        return (m_pHeadPointer + m_nCurSize) - m_pWritePointer;
    }

    CSockRecvBuffer::size_type CSockRecvBuffer::GetWriteableMax()
    {
        return m_nCurSize - GetReadableLen();
    }

    //��ջ���������
    void CSockRecvBuffer::Clear()
    {
        m_pReadPointer = m_pHeadPointer;
        m_pWritePointer = m_pHeadPointer;
        DeAlloc(m_nMinSize);
    }

    //����ָ�����ȵĿɶ�����
    void CSockRecvBuffer::Erase(CSockRecvBuffer::size_type nEraseLen)
    {
        if (0 == nEraseLen)
            return;

        size_t nDataLen = GetReadableLen();
        if (nEraseLen >= nDataLen)
            Clear();
        else
            m_pReadPointer += nEraseLen;
    }

    //��������
    CSockRecvBuffer::size_type CSockRecvBuffer::Read(char* pData, CSockRecvBuffer::size_type nReadLen)
    {
        //�����������ݿɶ�
        size_t nDataLen = GetReadableLen();
        if (0 == nDataLen)
            return 0;

        //��ȡ���ݲ���̬��������������
        nReadLen = nReadLen > nDataLen ? nDataLen : nReadLen;
        memcpy(pData, m_pReadPointer, nReadLen);
        Erase(nReadLen);
        return nReadLen;
    }

    //д������
    CSockRecvBuffer::size_type CSockRecvBuffer::Write(const char* pData, CSockRecvBuffer::size_type nWriteLen)
    {
        size_type nWroteLen = 0;

        //�����ǰ���������Ѿ��������ֵ - ֱ�ӷ���0
        size_type nDataLen = GetReadableLen();
        if (nDataLen >= m_nMaxSize)
            return 0;

        //�ж��ڲ��ƶ�����дָ���������Ƿ���Ȼ���㹻�Ŀռ�д������
        //���㹻�Ŀռ�д��������ֱ��д�뵽������
        if (nWriteLen > GetWriteableLen())
        {
            if (GetWriteableMax() >= nWriteLen)
            {
                //ͨ���ƶ�����дָ���λ���ڵ�ǰ��������ø�����ڴ��
                Align();

                //�����������ȫ��д�뵽��������
                nWroteLen = nWriteLen;
            }
            else
            {
                size_type nReqLen = 0;
                if ((nWriteLen + nDataLen) >= m_nMaxSize)
                {
                    //������д������ݳ��� + �ѻ�������ݳ��ȴ��ڻ��������������
                    //ֱ��������󳤶ȵ��ڴ�
                    nReqLen = m_nMaxSize;

                    //�����˻�������������ƣ�ֻ��д�벿���������ݵ���������
                    nWroteLen = m_nMaxSize - nDataLen;
                }
                else
                {
                    //������д������ݳ��� + �ѻ�������ݳ��� ���������������������
                    //�����㹻���浱ǰ����д�볤�� + �ѻ��泤�ȵ��ڴ漴��
                    nReqLen = nDataLen + nWriteLen;

                    //�����������ȫ��д�뵽��������
                    nWroteLen = nWriteLen;
                }

                if (ReAlloc(nReqLen) == 0)
                    return 0;
            }
        }
        else
        {
            nWroteLen = nWriteLen;
        }

        memcpy(m_pWritePointer, pData, nWroteLen);
        m_pWritePointer += nWroteLen;
        return nWroteLen;
    }

    //�ƶ��ɶ����ݵ�������ͷ��
    void CSockRecvBuffer::Align()
    {
        //�����пɶ������ƶ���ͷ��
        size_type nDataLen = GetReadableLen();
        if (nDataLen > 0)
            memmove(m_pHeadPointer, m_pReadPointer, nDataLen);

        //���ö���дָ��
        m_pReadPointer = m_pHeadPointer;
        m_pWritePointer = m_pHeadPointer + nDataLen;         
    }

    //����������
    CSockRecvBuffer::size_type CSockRecvBuffer::DeAlloc(CSockRecvBuffer::size_type nReqSize)
    {
        //����Ŀ�곤�ȱȵ�ǰʵ�ʳ���Ҫ��
        if (nReqSize >= m_nCurSize)
            return 0;

        if (nReqSize < m_nMinSize)
            nReqSize = m_nMinSize;

        //�������ȴ��ڵ�ǰ����
        size_t nDataSize = GetReadableLen();
        if (nReqSize >= m_nCurSize || nReqSize < nDataSize)
            return 0;

        //������ʵ���������
        size_t nNewSize = m_nCurSize;
        for (; nNewSize > nReqSize; nNewSize /= 2)
        {
            if ((nNewSize / 2) < nDataSize)
            {
                break;
            }
        }

        return Alloc(nNewSize > m_nMinSize ? nNewSize : m_nMinSize);
    }

    //���仺����
    CSockRecvBuffer::size_type CSockRecvBuffer::ReAlloc(CSockRecvBuffer::size_type nReqSize)
    {
        //����Ŀ�곤�ȱȵ�ǰʵ�ʳ���ҪС
        if (nReqSize <= m_nCurSize)
            return 0;

        if (nReqSize > m_nMaxSize)
            nReqSize = m_nMaxSize;

        //������ʵ����䳤��
        size_t nNewSize = m_nCurSize;
        while (nReqSize > nNewSize)
            nNewSize *= 2;

        return Alloc(nNewSize < m_nMaxSize ? nNewSize : m_nMaxSize);
    }

    //�����ڴ�
    CSockRecvBuffer::size_type CSockRecvBuffer::Alloc(CSockRecvBuffer::size_type nAllocSize)
    {
        //���䳤�ȳ�������
        if (nAllocSize < m_nMinSize || nAllocSize > m_nMaxSize)
            return 0;

        if (m_pHeadPointer)
        {
            char* pSwapBuff = m_pHeadPointer;
            char* pNewBuff = m_pRuntimeAlloctor->Alloc(nAllocSize);

            if (pNewBuff)
            {
                size_type nDataSize = GetReadableLen();
                if (nDataSize > 0)
                {
                    memcpy(pNewBuff, m_pReadPointer, nDataSize);
                }

                m_nCurSize = nAllocSize;
                m_pHeadPointer = pNewBuff;
                m_pReadPointer = m_pHeadPointer;
                m_pWritePointer = m_pHeadPointer + nDataSize;
                m_pRuntimeAlloctor->DeAlloc(pSwapBuff);
                return nAllocSize;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            m_pHeadPointer = m_pRuntimeAlloctor->Alloc(nAllocSize);

            if (m_pHeadPointer)
            {
                m_nCurSize = nAllocSize;
                m_pReadPointer = m_pHeadPointer;
                m_pWritePointer = m_pHeadPointer;
                return nAllocSize;
            }
            else
            {
                return 0;
            }
        }
    }
}

