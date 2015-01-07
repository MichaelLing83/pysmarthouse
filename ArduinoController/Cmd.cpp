/*
*/
#include "Cmd.h"
#ifdef USE_RELAY
    #include "Relay.h"
    extern Relay relay;
#endif

/****************************************************************************/
Cmd::Cmd(String __cmdS):
    _cmdS(__cmdS)
{
}

void Cmd::execute(void)
{
    for (int i=0;i<_cmdS.length();i++) {
        switch (_cmdS.charAt(i)) {
        #ifdef USE_RELAY
        case '0': relay.on(); break;
        case '1': relay.off(); break;
        #endif
        case 'd': delay(500); break;
        default:
            continue;
        }
    }
}
