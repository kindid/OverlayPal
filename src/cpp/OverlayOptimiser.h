//
// This file is part of OverlayPal ( https://github.com/michel-iwaniec/OverlayPal )
// Copyright (c) 2021 Michel Iwaniec.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once
#ifndef OVERLAY_OPTIMISER_H
#define OVERLAY_OPTIMISER_H

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

#include "ImageUtils.h"
#include "GridLayer.h"

class OverlayOptimiser
{
public:

    class Error: public std::runtime_error
    {
    public:
        Error(const std::string& description):
            std::runtime_error(description)
        {}
    };

    struct Sprite
    {
        int x;
        int y;
        int p;
        std::set<uint8_t> colors;
    };

    OverlayOptimiser();

    void setExecutablePath(const std::string& executablePath);
    void setWorkPath(const std::string& workPath);

    std::string exePathFilename(const std::string& exeFilename) const;
    std::string workPathFilename(const std::string& workFilename) const;

    void convert(const Image2D& image,
                 uint8_t backgroundColor,
                 int gridCellColorLimit,
                 int maxBackgroundPalettes,
                 int maxSpritePalettes,
                 int maxSpritesPerScanline,
                 int timeOut);

    bool conversionSuccessful() const;

    Image2D outputImageBackground() const;

    Image2D outputImageOverlay() const;

    Image2D outputImage() const;

    Image2D remapColors(const Image2D& image,
                        const GridLayer& layer,
                        const std::vector<std::set<uint8_t>>& palettes,
                        const Array2D<uint8_t>& paletteIndices) const;

    const std::vector<std::set<uint8_t>>& palettes() const;

    void zeroEmptyPaletteIndices(Array2D<uint8_t>& paletteIndices, const GridLayer& layer);

    const Array2D<uint8_t>& debugPaletteIndicesBackground() const;

    const GridLayer& layerBackground() const;

    const GridLayer& layerOverlay() const;

    std::vector<OverlayOptimiser::Sprite> spritesOverlay() const;

protected:

    void writeCmplDataFile(const GridLayer& layer, int gridCellColorLimit, int maxBackgroundPalettes, int maxSpritePalettes, int maxRowSize, const std::string& filename);
    void writeCmplLayerData(std::ofstream& f, const std::string& name, const GridLayer& layer, std::function<int(int, int, int)> const& callback);

    void runCmplProgram(const std::string& inputFilename,
                        const std::string& outputFilename,
                        const std::string& solutionCsvFilename,
                        int timeOut);

    static void parseSolutionValue(const std::string& line, std::vector<int>& indices, int& value);

    bool parseCmplSolution(const std::string& csvFilename,
                           std::vector<std::set<uint8_t>>& palettes,
                           GridLayer& colorsBackground,
                           GridLayer& colorsOverlay,
                           Array2D<uint8_t>& paletteIndicesBackground,
                           bool secondPass);

    bool consistentLayers(const Image2D& image,
                          const GridLayer& layer,
                          const std::vector<std::set<uint8_t>>& palettes,
                          const Array2D<uint8_t>& paletteIndices,
                          uint8_t backgroundColor);

    bool convertFirstPass(const Image2D& image,
                          int gridCellColorLimit,
                          int maxBackgroundPalettes,
                          int maxSpritePalettes,
                          int maxRowSize,
                          int timeOut,
                          GridLayer& layer,
                          GridLayer& layerBackground,
                          GridLayer& layerOverlay,
                          std::vector<std::set<uint8_t>>& palettesBG,
                          Array2D<uint8_t>& paletteIndicesBackground);

    bool convertSecondPass(int gridCellColorLimit,
                           int maxSpritePalettes,
                           int maxSpritesPerScanline,
                           int timeOut,
                           GridLayer& layer,
                           GridLayer& layerBackground,
                           GridLayer& layerOverlay,
                           std::vector<std::set<uint8_t>>& palettes,
                           Array2D<uint8_t>& paletteIndicesBackground);

    void fillMissingPaletteGroups(std::vector<std::set<uint8_t>>& palettes);

private:
    std::string mExecutablePath;
    std::string mWorkPath;
    bool mConversionSuccessful;
    uint8_t mBackgroundColor;
    Image2D mOutputImage;
    Image2D mOutputImageBackground;
    Image2D mOutputImageOverlay;
    Image2D mOutputImageOverlayFree;
    std::vector<std::set<uint8_t>> mPalettes;
    std::unordered_map<uint8_t, uint8_t> mRemappingForward;
    GridLayer mLayerBackground;
    GridLayer mLayerOverlay;
    GridLayer mLayerOverlayFree;
    Array2D<uint8_t> mPaletteIndicesBackground;
    Array2D<uint8_t> mPaletteIndicesOverlay;
    const int SpriteWidth = 8;
    const int GridCellWidth = 16;
    const int GridCellHeight = 16;
    const size_t PaletteGroupSize = 4;
    const char* firstPassProgramInputFilename = "FirstPass.cmpl";
    const char* firstPassProgramOutputFilename = "FirstPass_withTimeOut.cmpl";
    const char* firstPassSolutionFilename = "firstpass_output.csv";
    const char* firstPassDataFilename = "firstpass_input.cdat";
    const char* secondPassProgramInputFilename = "SecondPass.cmpl";
    const char* secondPassProgramOutputFilename = "SecondPass_withTimeOut.cmpl";
    const char* secondPassSolutionFilename = "secondpass_output.csv";
    const char* secondPassDataFilename = "secondpass_input.cdat";
};

#endif // OVERLAY_OPTIMISER_H
