#include "../Utils.h"

namespace lv2 
{
	class console 
    {
	public:
		static console* get_instance( void )  
        { 
            return STATIC_FN( &get_instance, LibLV2.lv2_console_get_instance_opd )();
        }

        int putc( 
            int Character
        )         
        { 
            return CLASS_FN( &console::putc, this, LibLV2.lv2_console_putc_opd )( Character );
        }

        void write( 
            const char* Text,
            int         Length
        ) 
        {
            CLASS_FN( &console::write, this, LibLV2.lv2_console_write_opd )( Text, Length );
        }

		void write_async( 
            const char* Text,
            int         Length
        ) 
		{ 
			CLASS_FN( &console::write_async, this, LibLV2.lv2_console_write_async_opd )( Text, Length );
		}

        int flush( void )
        {
            return CLASS_FN( &console::flush, this, LibLV2.lv2_console_flush_opd )();
        }
	};
}