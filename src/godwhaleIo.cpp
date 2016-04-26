
#if defined(GODWHALE_CLUSTER_SLAVE)
#include "godwhaleIo.hpp"
#include "common.hpp"
#include "asio_socket_streambuf.hpp"

#include <boost/asio/ip/tcp.hpp>

// logging�p��hack�Bstreambuf�������hook���Ă��܂��Βǉ��R�[�h�Ȃ��ŕ��ʂ�
// cin����̓��͂�cout�ւ̏o�͂��t�@�C���Ƀ��_�C���N�g�ł���B
// cf. http://groups.google.com/group/comp.lang.c++/msg/1d941c0f26ea0d81
struct Tie : public std::streambuf
{
    Tie(std::streambuf* buf_, std::streambuf* remote_) 
        : buf(buf_), remote(remote_), last('\n') {}

    int sync() { return buf->pubsync(), remote->pubsync(); }
    int_type uflow() { return remote->sbumpc(); }
    int_type underflow() { return write(remote->sgetc(), ">> "); }
    int_type overflow(int_type c) { return write(remote->sputc(traits_type::to_char_type(c)), "<< "); }

    std::streamsize xsgetn(char *ptr, std::streamsize count) {
        std::streamsize ret = remote->sgetn(ptr, count);
        writen(ptr, count, ">> ");
        return ret;
    }

    std::streamsize xsputn(const char *ptr, std::streamsize count) {
        writen(ptr, count, "<< ");
        return remote->sputn(ptr, count);
    }
    
    int_type write(int_type c, const char *prefix) {
        if (c == traits_type::eof()) {
            return c;
        }
        if (last == '\n') buf->sputn(prefix, 3);
        buf->sputc(traits_type::to_char_type(c));
        return (last = c);
    }

    void writen(const char *ptr, std::streamsize count, const char *prefix) {
        std::streamsize curr, prev=0;

        /*if (strstr(ptr, "optim")) {
            prev = 0;
        }*/

        if (last == '\n') buf->sputn(prefix, 3);
        for (curr = 0; curr < count - 1; ++curr) {
            if (ptr[curr] == '\n') {
                buf->sputn(&ptr[prev], curr - prev + 1);
                buf->sputn(prefix, 3);
                prev = curr + 1;
            }
        }

        if (prev < count) buf->sputn(&ptr[prev], count - prev);
        last = ptr[count - 1];
    }

    std::streambuf *buf, *remote; // �W�����o�� , �����[�g
    int_type last;
};

struct GodwhaleIO
{
    GodwhaleIO()
        : cinbuf(std::cout.rdbuf()), coutbuf(std::cin.rdbuf())
        , in(std::cout.rdbuf(), &sockstream)
        , out(std::cout.rdbuf(), &sockstream) {}
    ~GodwhaleIO() { close(); }

    void start(const std::string &host, const std::string &port) {
        std::cout << "connect to " << host << ":" << port << std::endl;

        // �ڑ��ł���܂Ń��[�v����
        while (sockstream.connect(host, port) == nullptr) {
            std::cout << "retry..." << std::endl;
            _sleep(10 * 1000);
        }

        std::cout << "succeeded to connect" << std::endl;
        std::cin.rdbuf(&in);
        std::cout.rdbuf(&out);
    }

    void close() {
        if (sockstream.is_open()) {
            std::cout.rdbuf(coutbuf);
            std::cin.rdbuf(cinbuf);
            sockstream.close();
            std::cout << "connection closed" << std::endl;
        }
    }

private:
    boost::asio::asio_socket_streambuf<boost::asio::ip::tcp> sockstream;
    std::streambuf *coutbuf, *cinbuf;
    Tie in, out;   // �W�����͂ƃt�@�C���A�W���o�͂ƃt�@�C���̂Ђ��t��
};


static std::unique_ptr<GodwhaleIO> io;

void startGodwhaleIo(const std::string &host, const std::string &port)
{
    io.reset(new GodwhaleIO);
    io->start(host, port);
}

void closeGodwhaleIo()
{
    if (io != nullptr) {
        io->close();
        io = nullptr;
    }
}

#endif
