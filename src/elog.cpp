#include "../inc/elog.h"

// Constructor
eLog::eLog()
{
    char            _Message[256];
    const char*     _FileName       = __FILENAME__;
    const char*     _FunctionName   = __FUNCTION__;

    #if LOG_CONSOLE_OR_FILE == 0

        snprintf (_Message, 255, "Logging has been successfully started.");

        writeLogToConsole(_FileName,
                                std::to_string(gettid()),
                                _FunctionName,
                                std::to_string(__LINE__),
                                LevelNames[ 0 ],
                                _Message);

    #else

        if( !LogFile.is_open() )
        {
            LogFile.open(getLogFileFullName());

            addLogHeadToFile();

            snprintf (_Message, 255, "Logging has been successfully started. Max log file size: %dKB", MAX_FILE_SIZE);

            writeLogToFile(addSpacesToConstChar(_FileName, MAX_FILE_NAME_SIZE),
                            addSpacesToUnsignedInt(gettid(), MAX_TID_SIZE),
                            addSpacesToConstChar(_FunctionName, MAX_FUNC_NAME_SIZE),
                            addSpacesToUnsignedInt(__LINE__, MAX_LINE_SIZE),
                            addSpacesToConstChar(LevelNames[ 0 ], MAX_LEVEL_SIZE),
                            _Message);
        }

    #endif
}

// Destructor
eLog::~eLog()
{
    char            _Message[256];
    const char*     _FileName       = __FILENAME__;
    const char*     _FunctionName   = __FUNCTION__;

    #if LOG_CONSOLE_OR_FILE == 0
        snprintf (_Message, 255, "Logging has been successfully terminated.");

        writeLogToConsole(_FileName,
                                std::to_string(gettid()),
                                _FunctionName,
                                std::to_string(__LINE__),
                                LevelNames[ 0 ],
                                _Message);
    #else
        snprintf (_Message, 255, "Logging has been successfully terminated. Total log file: %s", LogFileNameInfix.c_str());

        writeLogToFile(addSpacesToConstChar(_FileName, MAX_FILE_NAME_SIZE),
                        addSpacesToUnsignedInt(gettid(), MAX_TID_SIZE),
                        addSpacesToConstChar(_FunctionName, MAX_FUNC_NAME_SIZE),
                        addSpacesToUnsignedInt(__LINE__, MAX_LINE_SIZE),
                        addSpacesToConstChar(LevelNames[ 0 ], MAX_LEVEL_SIZE),
                        _Message);

        LogFile.close();
    #endif
}

// Write a log message to log file
void eLog::writeLogToFile(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message)
{
    if( LogFile.is_open() )
    {
        LogFile << "[" << currentDateTime() << "]";
        LogFile << "[" << _FileName << "]";
        LogFile << "[" << _TID << "]";
        LogFile << "[" << _FunctionName << "]";
        LogFile << "[" << _Line << "]";
        LogFile << "[" << _LevelNames  << "]" << ": ";
        LogFile << _Message << std::endl;
        LogFile.flush();
    }
}

// Write a log message to console
void eLog::writeLogToConsole(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message)
{
    LogConsole << "[" << currentDateTime() << "]";
    LogConsole << "[" << _FileName << "]";
    LogConsole << "[" << _TID << "]";
    LogConsole << "[" << _FunctionName << "]";
    LogConsole << "[" << _Line << "]";
    LogConsole << "[" << _LevelNames << "]" << ": ";
    LogConsole << _Message << std::endl;
}

// Write a head to log file
void eLog::addLogHeadToFile()
{
    if( LogFile.is_open() )
    {
        LogFile << "[" << "            Date / Time"<< "]";
        LogFile << "[" << addSpacesToConstChar("File", MAX_FILE_NAME_SIZE)<< "]";
        LogFile << "[" << addSpacesToConstChar("TID", MAX_TID_SIZE) << "]";
        LogFile << "[" << addSpacesToConstChar("Function", MAX_FUNC_NAME_SIZE) << "]";
        LogFile << "[" << addSpacesToConstChar("Line", MAX_LINE_SIZE) << "]";
        LogFile << "[" << addSpacesToConstChar("Level", MAX_LEVEL_SIZE) << "]" << ": ";
        LogFile << "[" << "Message" << "]" << std::endl;
        LogFile.flush();
    }
}

// Create a new log file
void eLog::changeFile()
{
    uint32_t _FileSize = fileSize((getLogFileFullName()).c_str());

    if( _FileSize >= MAX_FILE_SIZE )
    {
        LogFile.close();

        LogFileNameInfix = std::to_string(std::stoi(LogFileNameInfix)+1);

        LogFile.open(getLogFileFullName());

        addLogHeadToFile();
    }
}

std::string eLog::currentDateTime()
{
    const auto              p1 = std::chrono::system_clock::now();
    uint16_t                milliseconds = (std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count())%1000;
    time_t                  now = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
    struct tm               tstruct = *localtime(&now);
    char                    buf[30];

    strftime(buf, sizeof(buf), "%Y.%m.%d-%X", &tstruct);

    if( milliseconds < 10 )
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), ".00%d", milliseconds);
    else if( 10 <= milliseconds && milliseconds < 100)
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), ".0%d", milliseconds);
    else
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), ".%d", milliseconds);

    return buf;
}

std::string eLog::getLogFileFullName()
{
    return LogFilePath + LogFileNamePrefix + LogFileNameInfix + LogFileNameSuffix;
}

std::ifstream::pos_type eLog::fileSize(const char* fName)
{
    std::ifstream file(fName, std::ifstream::ate | std::ifstream::binary);

    return file.tellg();
}

// Add spaces to const char*
// It is used to change the function name and file name.
std::string eLog::addSpacesToConstChar(const char* getChar, uint8_t maxSize)
{
    std::string     _StringWithSpace;

    if( strlen(getChar) < maxSize )
        for (size_t i = 0; i < maxSize - strlen(getChar); i++)
            _StringWithSpace = _StringWithSpace + " ";

    _StringWithSpace = _StringWithSpace + getChar;

    return _StringWithSpace;
}

// Add spaces to unsigned int
std::string eLog::addSpacesToUnsignedInt(unsigned int getInt, uint8_t maxSize)
{
    std::string     _StringWithSpace;
    size_t          _IntLength = std::to_string(getInt).length();

    if( _IntLength < maxSize )
        for (size_t i = 0; i < maxSize - _IntLength; i++)
            _StringWithSpace = _StringWithSpace + " ";

    _StringWithSpace = _StringWithSpace + std::to_string(getInt);

    return _StringWithSpace;
}

eLog _eLog;