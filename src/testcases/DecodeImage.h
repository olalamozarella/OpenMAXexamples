#ifndef DECODEIMAGE_H
#define DECODEIMAGE_H

#include <string>

/**
 * @brief class DecodeImage - decodes image to raw image data. Input file format has to be decode-able by decoder component.
 */
class DecodeImage
{
public:
    DecodeImage();
    virtual ~DecodeImage();

    /**
     * @brief Init - initializes openmax core, components and opens/creates files
     * @return true if successful, false otherwise
     */
    bool Init( std::string inputFileName, std::string outputFileName );

    /**
     * @brief ReInit - closes currently open files and opens/creates files from parameter
     * @note openmax core and components are not destroyed
     * @return true if successful, false otherwise
     */
    bool ReInit( std::string inputFileName, std::string outputFileName );

    /**
     * @brief Run - contains main application logic
     */
    void Run();

    /**
     * @brief Destroy - closes input/output files, deinitializes and destroys openmax components and core
     */
    void Destroy();

private:
    class DataClass;
    DataClass* d;
};

#endif // DECODEIMAGE_H
