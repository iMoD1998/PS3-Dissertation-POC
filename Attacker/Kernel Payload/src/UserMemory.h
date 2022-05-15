#include "LV2/Process.h"

//
// Utils for managing user mapped pages.
//
template<typename T>
class UserMappedPointer 
{
public:
    UserMappedPointer( uint8_t* KernelPtr, uint32_t UserPtr, size_t Size = 1 ) :
        KernelPtr( KernelPtr ),
        UserPtr( UserPtr ),
        Size( Size )
    {}

    template<typename R>
    R UserAs()       const { return (R)this->UserPtr; }
    T* User()        const { return (T*)this->UserPtr;  }
    T* Kernel()      const { return (T*)this->KernelPtr; }
    T& operator * () const { return *this->Kernel(); }
    T& operator * ()       { return *this->Kernel(); }

    T* operator-> () const { return this->Kernel(); }
    T* operator-> ()       { return this->Kernel(); }

    uint8_t* const KernelPtr;
    const uint32_t UserPtr;
    size_t         Size;
};

class UserMappedMemory 
{
public:
    UserMappedMemory( void* KernelPage, uint32_t UserPage, size_t MaxSize ) :
            KernelPage( (uint8_t*)KernelPage ),
            UserPage( UserPage ),
            MaxSize( MaxSize ),
            CurrentSize( 0 )
    {}

    template<typename T>
    UserMappedPointer<T> Get( 
        size_t Count
    )
    {
        const auto RequestedSize = sizeof(T) * Count;
        
        if( ( RequestedSize + this->CurrentSize ) < this->MaxSize )
        {
            auto NewAlloc = UserMappedPointer<T>( &this->KernelPage[ this->CurrentSize ], UserPage + this->CurrentSize, RequestedSize );

            this->CurrentSize += RequestedSize;

            DEBUG_PRINT( "UserMappedPointer: NewAlloc: %llX RequestedSize: %X CurrentSize: %X\n", NewAlloc.KernelPtr, RequestedSize, this->CurrentSize );

            return NewAlloc;
        }

        return UserMappedPointer<T>( NULL, NULL, 0 );
    }

    uint8_t* const KernelPage;
    const uint32_t UserPage;
    size_t         CurrentSize;
    const size_t   MaxSize;
};

UserMappedMemory AllocateUserPage( 
    lv2::process* Process,
    size_t        Size,
    uint64_t      ExportFlags = 0x40000
);

void FreeUserModePages( 
    lv2::process*     Process,
    UserMappedMemory& MemoryManager
);

//
// Allocates kernel pages and maps them to a usermode address.
//
void AllocateUserModePagesWithCallback( 
    lv2::process* Process,
    size_t        Size,
    void          (*Callback)( UserMappedMemory& Memory )
);