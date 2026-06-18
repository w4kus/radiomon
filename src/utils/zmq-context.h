#pragma once

#include <zmq.hpp>
#include <memory>

namespace util {

template<typename T>
class zmq_stream
{
public:
    zmq_stream() = delete;
    zmq_stream(const zmq_stream&) = delete;
    zmq_stream& operator= (const zmq_stream&) = delete;

    zmq_stream(const int d, const int size)
    {
        m_buff =  std::make_unique<T[]>(size);
        m_mBuff = std::make_unique<zmq::mutable_buffer>(m_buff.get(), size * sizeof(T));
        m_sock = std::make_unique<zmq::socket_t>(m_ctx, d);
    }

    auto &sock() { return *m_sock; }
    auto recv(zmq::recv_flags flags = zmq::recv_flags::none) { return m_sock->recv(*m_mBuff, flags); }
    auto send(zmq::send_flags flags = zmq::send_flags::none) { return m_sock->send(zmq::const_buffer(*m_buff), flags); }

    auto *data() { return m_mBuff->data(); }
    const int size(zmq::recv_buffer_result_t &res) { return res.value().size / sizeof(T); }

private:
    std::unique_ptr<T[]> m_buff;
    
    zmq::context_t m_ctx;
    std::unique_ptr<zmq::socket_t> m_sock;
    std::unique_ptr<zmq::mutable_buffer> m_mBuff;
};
}
