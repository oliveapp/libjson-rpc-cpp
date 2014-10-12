/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    main.cpp
 * @date    29.09.2013
 * @author  Peter Spiess-Knafl <peter.knafl@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include <string>
#include <iostream>

#include <jsonrpccpp/server.h>
#include <jsonrpccpp/client.h>

#include <argtable2.h>

#include "client/cppclientstubgenerator.h"
#include "server/cppserverstubgenerator.h"

#define EXIT_ERROR(X) cerr << X << endl;arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));return 1;

using namespace std;
using namespace jsonrpc;

int main(int argc, char** argv)
{
    struct arg_lit *help        = arg_lit0("h","help", "print this help and exit");
    struct arg_file *inputfile  = arg_file0("i", "input", "<inputfile>.json", "path of input specification file");
    struct arg_str *classname   = arg_str0("c", "class", NULL, "name of the class");
    struct arg_str *server      = arg_str0("s", "server", "<filename>.h", "name of the server stubfile");
    struct arg_str *cppclient   = arg_str0(NULL, "cpp", "<filename>.h", "name of the cpp client stubfile");
    struct arg_end *end         = arg_end(20);
    void* argtable[] = {help, inputfile, classname, server, cppclient, end};

    if (arg_parse(argc,argv,argtable) > 0)
    {
        //  cerr << "Invalid arguments: try --help for more information" << endl;
        arg_print_errors(stderr, end, argv[0]);
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
        return 1;
    }

    if (help->count > 0 || argc == 1)
    {
        cout << "Usage: " << argv[0] << " "; cout.flush();
        arg_print_syntax(stdout,argtable,"\n"); cout << endl;
        arg_print_glossary_gnu(stdout, argtable);
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
        return 0;
    }

    if (inputfile->count == 0 || classname->count == 0)
    {
        EXIT_ERROR("Invalid arguments: -i|--input and -c|--class parameter must be provided")
    }

    std::vector<Procedure> procedures = SpecificationParser::GetProceduresFromFile(inputfile->filename[0]);


    try {
        if (server->count > 0)
        {
            CodeGenerator cg(server->sval[0]);
            CPPServerStubGenerator serverstub(classname->sval[0], procedures, cg);
            serverstub.generateStub();
        }

        if (cppclient->count > 0)
        {
            CodeGenerator cg(cppclient->sval[0]);
            CPPClientStubGenerator cppclientstub(classname->sval[0], procedures, cg);
            cppclientstub.generateStub();

        }
    }
    catch (const JsonRpcException &ex)
    {
        EXIT_ERROR(ex.what())
    }
    return 0;

}
