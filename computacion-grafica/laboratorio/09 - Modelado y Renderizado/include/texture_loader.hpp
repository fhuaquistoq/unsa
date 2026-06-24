#pragma once

#include <GL/glut.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace texture_loader
{

    struct LoadedTexture
    {
        GLuint id = 0;
        bool loaded = false;
        std::string path;
        std::string message;
    };

    inline bool fileExists(const std::string &path)
    {
        std::ifstream stream(path, std::ios::binary);
        return stream.good();
    }

    inline std::uint16_t readU16(const unsigned char *bytes)
    {
        return static_cast<std::uint16_t>(bytes[0]) |
               (static_cast<std::uint16_t>(bytes[1]) << 8U);
    }

    inline std::uint32_t readU32(const unsigned char *bytes)
    {
        return static_cast<std::uint32_t>(bytes[0]) |
               (static_cast<std::uint32_t>(bytes[1]) << 8U) |
               (static_cast<std::uint32_t>(bytes[2]) << 16U) |
               (static_cast<std::uint32_t>(bytes[3]) << 24U);
    }

    inline LoadedTexture loadBmpTexture(const std::vector<std::string> &candidatePaths)
    {
        for (const std::string &candidate : candidatePaths)
        {
            if (!fileExists(candidate))
            {
                continue;
            }

            std::ifstream file(candidate, std::ios::binary);
            if (!file)
            {
                return {0, false, candidate, "Failed to open BMP file."};
            }

            unsigned char header[54] = {};
            file.read(reinterpret_cast<char *>(header), sizeof(header));
            if (file.gcount() != static_cast<std::streamsize>(sizeof(header)))
            {
                return {0, false, candidate, "BMP header is incomplete."};
            }

            if (header[0] != 'B' || header[1] != 'M')
            {
                return {0, false, candidate, "Unsupported file signature. Expected BMP."};
            }

            const std::uint32_t dataOffset = readU32(header + 10);
            const std::uint32_t dibHeaderSize = readU32(header + 14);
            const std::int32_t width = static_cast<std::int32_t>(readU32(header + 18));
            const std::int32_t height = static_cast<std::int32_t>(readU32(header + 22));
            const std::uint16_t planes = readU16(header + 26);
            const std::uint16_t bitsPerPixel = readU16(header + 28);
            const std::uint32_t compression = readU32(header + 30);

            if (dibHeaderSize < 40U || planes != 1U || bitsPerPixel != 24U || compression != 0U)
            {
                return {0, false, candidate, "Only uncompressed 24-bit BMP files are supported."};
            }
            if (width <= 0 || height == 0)
            {
                return {0, false, candidate, "Invalid BMP dimensions."};
            }

            const int absoluteHeight = height > 0 ? height : -height;
            const std::size_t rowStride = static_cast<std::size_t>((width * 3 + 3) & ~3);
            std::vector<unsigned char> bgrData(rowStride * static_cast<std::size_t>(absoluteHeight));

            file.seekg(static_cast<std::streamoff>(dataOffset), std::ios::beg);
            file.read(reinterpret_cast<char *>(bgrData.data()), static_cast<std::streamsize>(bgrData.size()));
            if (!file)
            {
                return {0, false, candidate, "BMP pixel data is incomplete."};
            }

            std::vector<unsigned char> rgbData(static_cast<std::size_t>(width) *
                                               static_cast<std::size_t>(absoluteHeight) * 3U);

            for (int y = 0; y < absoluteHeight; ++y)
            {
                const int sourceRow = height > 0 ? (absoluteHeight - 1 - y) : y;
                const unsigned char *row = &bgrData[static_cast<std::size_t>(sourceRow) * rowStride];

                for (int x = 0; x < width; ++x)
                {
                    const std::size_t sourceIndex = static_cast<std::size_t>(x) * 3U;
                    const std::size_t targetIndex =
                        (static_cast<std::size_t>(y) * static_cast<std::size_t>(width) +
                         static_cast<std::size_t>(x)) *
                        3U;

                    rgbData[targetIndex + 0U] = row[sourceIndex + 2U];
                    rgbData[targetIndex + 1U] = row[sourceIndex + 1U];
                    rgbData[targetIndex + 2U] = row[sourceIndex + 0U];
                }
            }

            GLuint textureId = 0;
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

            gluBuild2DMipmaps(GL_TEXTURE_2D,
                              GL_RGB,
                              width,
                              absoluteHeight,
                              GL_RGB,
                              GL_UNSIGNED_BYTE,
                              rgbData.data());

            return {textureId, true, candidate, "Loaded successfully."};
        }

        return {0, false, "", "Texture file was not found in the configured search paths."};
    }

    inline LoadedTexture loadNamedTexture(const std::string &name)
    {
        const std::vector<std::string> paths = {
            "assets/" + name,
            "textures/" + name,
            name,
        };

        LoadedTexture texture = loadBmpTexture(paths);
        if (texture.loaded)
        {
            std::cout << "[texture] " << name << " -> " << texture.path << '\n';
        }
        else
        {
            std::cout << "[texture] Missing " << name << ": " << texture.message << '\n';
        }

        return texture;
    }

} // namespace texture_loader
