#pragma once

#include "raylib.h"
#include <ui/UIButton.hpp>
#include <vector>

class PixelButton : public UIButton
{
private:

    struct PixelCell
    {
        int x;
        int y;
    };

    std::vector<PixelCell> _cells;

    int _blockSize = 12;

public:

    PixelButton(const std::string& text, Rectangle rect)
        : UIButton(text, rect)
    {
        generateCells();
    }

private:

    void addCluster(int x, int y)
    {
        _cells.push_back({x,y});

        float n = fmodf(
            sinf(x * 12.9898f + y * 78.233f) * 43758.5453f,
            1.0f);

        if(n > 0.6f) _cells.push_back({x+1,y});
        if(n > 0.7f) _cells.push_back({x,y+1});
        if(n > 0.8f) _cells.push_back({x+1,y+1});
    }

    void generateCells()
    {
        Rectangle r = baseRect();

        int cols = r.width / _blockSize;
        int rows = r.height / _blockSize;

        for(int y = -2; y <= rows + 1; y++)
        {
            for(int x = -2; x <= cols + 1; x++)
            {
                bool outside =
                    x < 0 || x >= cols ||
                    y < 0 || y >= rows;

                if(!outside)
                    continue;

                float noise =
                    fmodf(
                        sinf(x * 15.73f + y * 91.17f) *
                        43758.5453f,
                        1.0f);

                if(noise > 0.7f)
                    addCluster(x,y);
            }
        }
    }

public:

    void drawDefault() override
    {
        UIButton::drawDefault();

        Rectangle r = rect();

        Color c = hover()
            ? Color{255,220,120,100}
            : Color{0, 0, 0, 80};

        for(auto& cell : _cells)
        {
            float px = r.x + cell.x * _blockSize;
            float py = r.y + cell.y * _blockSize;

            DrawRectangle(px, py, _blockSize, _blockSize, c);
        }
    }
};