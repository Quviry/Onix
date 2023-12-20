// dear imgui: "null" example application
// (compile and link imgui, create context, run headless with NO INPUTS, NO GRAPHICS OUTPUT)
// This is useful to test building, but you cannot interact with anything here!
#include "src/Client.h"

#include <stdio.h>

int main(int argc, char** argv)
{

    if (argc == 1){
        Client client{};
        client.start();
    }
    else{
        printf("IMSOBAKA");
    }
    return 0;
}