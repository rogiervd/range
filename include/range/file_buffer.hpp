/*
Copyright 2015 Rogier van Dalen.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef RANGE_FILE_BUFFER_HPP_INCLUDED
#define RANGE_FILE_BUFFER_HPP_INCLUDED

#include <cstdio>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/info.hpp>


#include "buffer.hpp"

namespace range {

/** \brief
Exception that indicates an error while opening a file.
*/
struct file_open_error
: virtual std::ios_base::failure, virtual boost::exception
{
    file_open_error()
    : std::ios_base::failure ("Error opening file") {}
};

/** \brief
Exception that indicates an error while reading from a file.
*/
struct file_read_error
: virtual std::ios_base::failure, virtual boost::exception
{
    file_read_error()
    : std::ios_base::failure ("Error reading from file") {}
};

namespace file_producer_detail {

    /** \brief
    File source for use with Boost.IOStreams.

    This is a thin wrapper over the C API which behaves as it should: if an
    object is constructed, the file is open; if an error occurs, an exception is
    thrown.
    */
    class file_source {
        std::string file_name_;
        std::shared_ptr <FILE> handle_;

    public:
        /** \brief
        Open the file with name \a file_name.

        \throw file_open_error
            Iff an error occurs.
        */
        file_source (std::string file_name) {
            FILE * handle = std::fopen (file_name.c_str(), "rb");
            if (handle)
                this->handle_ = std::shared_ptr <FILE> (handle, std::fclose);
            else
                throw file_open_error() <<
                    boost::errinfo_errno (errno) <<
                    boost::errinfo_file_name (file_name);

            file_name_ = std::move (file_name);
        }

        typedef char char_type;
        typedef boost::iostreams::source_tag category;

        /** \brief
        Read a number of bytes into the buffer given.

        This implements a requirement of Boost.IOStreams Source.

        \throw file_read_error
            Iff an error occurs.
        */
        std::streamsize read (char * target, std::streamsize number) {
            std::size_t actually_read_num = std::fread (
                target, sizeof (char), number, handle_.get());
            if (std::ferror (handle_.get()))
                throw file_read_error() << boost::errinfo_errno (errno);
            return actually_read_num;
        }
    };

    class gzip_file_stream
    : public boost::iostreams::filtering_streambuf <boost::iostreams::input>
    {
        boost::iostreams::stream_buffer <file_producer_detail::file_source>
            underlying_;
    public:
        gzip_file_stream (std::string file_name)
        : underlying_ (std::move (file_name)) {
            this->push (boost::iostreams::gzip_decompressor());
            this->push (underlying_);
        }
    };

} // namespace file_producer_detail

template <class Char> class file_element_producer;

/** \brief
Open a file for reading and expose as a \ref buffer.

The file is opened in binary mode, i.e. no conversion of line endings is
performed.

This uses Boost.IOStreams, which you must explicitly link to if you use this
function.
*/
inline buffer <char> read_file (std::string const & file_name) {
    auto stream_buffer = utility::make_unique <
        boost::iostreams::stream_buffer <file_producer_detail::file_source>> (
            file_name);

    return range::buffer <char> (
        internal_element_producer <char, 256>::pointer::template
        construct <file_element_producer <char>> (std::move (stream_buffer)));
}

/** \brief
Open a file in Gzip format for reading and expose as a \ref buffer.
*/
inline buffer <char> read_gzip_file (std::string const & file_name) {
    auto stream_buffer = utility::make_unique <
        file_producer_detail::gzip_file_stream> (file_name);
    return range::buffer <char> (
        internal_element_producer <char, 256>::pointer::template
        construct <file_element_producer <char>> (std::move (stream_buffer)));
}

template <class Char> class file_element_producer
: public internal_element_producer <Char, 256>
{
    static constexpr std::size_t buffer_size = 256;
    typedef internal_element_producer <Char, buffer_size> base_type;
    typedef typename base_type::pointer pointer;

    typedef std::unique_ptr <std::basic_streambuf <Char>> stream_buffer_ptr;

    // Only the last producer needs and has access to the streambuf.
    stream_buffer_ptr stream_buffer_;

protected:
    virtual pointer get_next() {
        return pointer::template construct <file_element_producer> (
            std::move (stream_buffer_));
    }

    void fill() {
        auto count = stream_buffer_->sgetn (this->memory(), buffer_size);
        this->end_  = this->memory() + count;
    }

public:
    /**
    The buffer must be set to throw on errors.
    */
    file_element_producer (std::unique_ptr <std::streambuf> && stream_buffer)
    : stream_buffer_ (std::move (stream_buffer))
    { fill(); }
};

} // namespace range

#endif // RANGE_FILE_BUFFER_HPP_INCLUDED
