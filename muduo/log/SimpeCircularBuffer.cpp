#include <iostream>
#include <string>
#include <vector>
//http://www.cplusplus.com/forum/beginner/176072/
struct ring_buffer
{
    ring_buffer( std::size_t cap ) : buffer(cap) {}
    bool empty() const { return sz == 0 ; }
    bool full() const { return sz == buffer.size() ; }

    void push( std::string str )
    {
        if( last >= buffer.size() ) last = 0 ;
        buffer[last] = str ;
        ++last ;
        if( full() ) first = (first+1) %  buffer.size() ;
        else ++sz ;
    }

    std::string& operator[] ( std::size_t pos )
    {
        auto p = ( first + pos ) % buffer.size() ;
        return buffer[p] ;
    }

    std::ostream& print( std::ostream& stm = std::cout ) const
    {
        if( first < last )
            for( std::size_t i = first ; i < last ; ++i ) std::cout << buffer[i] << ' ' ;
        else
        {
            for( std::size_t i = first ; i < buffer.size() ; ++i ) std::cout << buffer[i] << ' ' ;
            for( std::size_t i = 0 ; i < last ; ++i ) std::cout << buffer[i] << ' ' ;
        }
        return stm ;
    }

private:
    std::vector<std::string> buffer ;
    std::size_t first = 0 ;
    std::size_t last = 0 ;
    std::size_t sz = 0 ;
};

int main()
{
    ring_buffer rb(8) ;

    for( int i = 10 ; i < 30 ; ++i )
    {
        rb.push( std::to_string(i) ) ;
        rb.print() << '\n' ;
    }
}
