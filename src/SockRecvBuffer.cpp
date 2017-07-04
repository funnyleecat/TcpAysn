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

    //清空缓冲区数据
    void CSockRecvBuffer::Clear()
    {
        m_pReadPointer = m_pHeadPointer;
        m_pWritePointer = m_pHeadPointer;
        DeAlloc(m_nMinSize);
    }

    //擦除指定长度的可读数据
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

    //读出数据
    CSockRecvBuffer::size_type CSockRecvBuffer::Read(char* pData, CSockRecvBuffer::size_type nReadLen)
    {
        //缓冲区无数据可读
        size_t nDataLen = GetReadableLen();
        if (0 == nDataLen)
            return 0;

        //读取数据并动态调整缓冲区长度
        nReadLen = nReadLen > nDataLen ? nDataLen : nReadLen;
        memcpy(pData, m_pReadPointer, nReadLen);
        Erase(nReadLen);
        return nReadLen;
    }

    //写入数据
    CSockRecvBuffer::size_type CSockRecvBuffer::Write(const char* pData, CSockRecvBuffer::size_type nWriteLen)
    {
        size_type nWroteLen = 0;

        //如果当前缓存数据已经到达最大值 - 直接返回0
        size_type nDataLen = GetReadableLen();
        if (nDataLen >= m_nMaxSize)
            return 0;

        //判断在不移动读、写指针的情况下是否仍然有足够的空间写入数据
        //有足够的空间写入数据则直接写入到缓冲区
        if (nWriteLen > GetWriteableLen())
        {
            if (GetWriteableMax() >= nWriteLen)
            {
                //通过移动读、写指针的位置在当前缓冲区获得更大的内存块
                Align();

                //将请求的数据全部写入到缓冲区内
                nWroteLen = nWriteLen;
            }
            else
            {
                size_type nReqLen = 0;
                if ((nWriteLen + nDataLen) >= m_nMaxSize)
                {
                    //当请求写入的数据长度 + 已缓存的数据长度大于缓冲区的最大限制
                    //直接申请最大长度的内存
                    nReqLen = m_nMaxSize;

                    //超过了缓冲区的最大限制，只能写入部分请求数据到缓冲区内
                    nWroteLen = m_nMaxSize - nDataLen;
                }
                else
                {
                    //当请求写入的数据长度 + 已缓存的数据长度 不超过缓冲区的最大限制
                    //申请足够缓存当前请求写入长度 + 已缓存长度的内存即可
                    nReqLen = nDataLen + nWriteLen;

                    //将请求的数据全部写入到缓冲区内
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

    //移动可读数据到缓冲区头部
    void CSockRecvBuffer::Align()
    {
        //将所有可读数据移动到头部
        size_type nDataLen = GetReadableLen();
        if (nDataLen > 0)
            memmove(m_pHeadPointer, m_pReadPointer, nDataLen);

        //重置读、写指针
        m_pReadPointer = m_pHeadPointer;
        m_pWritePointer = m_pHeadPointer + nDataLen;         
    }

    //收缩缓冲区
    CSockRecvBuffer::size_type CSockRecvBuffer::DeAlloc(CSockRecvBuffer::size_type nReqSize)
    {
        //收缩目标长度比当前实际长度要大
        if (nReqSize >= m_nCurSize)
            return 0;

        if (nReqSize < m_nMinSize)
            nReqSize = m_nMinSize;

        //收缩长度大于当前长度
        size_t nDataSize = GetReadableLen();
        if (nReqSize >= m_nCurSize || nReqSize < nDataSize)
            return 0;

        //计算合适的收缩长度
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

    //扩充缓冲区
    CSockRecvBuffer::size_type CSockRecvBuffer::ReAlloc(CSockRecvBuffer::size_type nReqSize)
    {
        //扩充目标长度比当前实际长度要小
        if (nReqSize <= m_nCurSize)
            return 0;

        if (nReqSize > m_nMaxSize)
            nReqSize = m_nMaxSize;

        //计算合适的扩充长度
        size_t nNewSize = m_nCurSize;
        while (nReqSize > nNewSize)
            nNewSize *= 2;

        return Alloc(nNewSize < m_nMaxSize ? nNewSize : m_nMaxSize);
    }

    //申请内存
    CSockRecvBuffer::size_type CSockRecvBuffer::Alloc(CSockRecvBuffer::size_type nAllocSize)
    {
        //扩充长度超出限制
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

