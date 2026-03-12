#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

// Classe utilitaire pour gérer différentes formes d'animations sprites :
// - frames individuelles
// - spritesheet en grille
// - strip horizontal
// - atlas JSON (TexturePacker, Aseprite, etc.)
class AnimatedSprite
{
private:
    // frames chargées individuellement
    std::vector<Texture2D *> _frames;

    // texture utilisée lorsqu'on travaille avec une spritesheet
    Texture2D *_sheet = nullptr;

    // rectangles des frames si on utilise un atlas JSON
    std::vector<Rectangle> _atlasFrames;

    // indique si l'animation utilise une spritesheet ou des textures séparées
    bool _isSheet = false;

    // dimensions d'une frame dans une spritesheet
    int _frameWidth = 0;
    int _frameHeight = 0;

    // nombre total de frames dans la spritesheet
    int _sheetFrameCount = 0;

    // frame actuellement affichée
    int _currentFrame = 0;

    // timer interne pour contrôler la vitesse d'animation
    float _timer = 0.f;
    float _frameTime = 0.05f;

    // indique si le sprite est prêt à être utilisé
    bool _ready = false;

    // dimensions de la grille de la spritesheet
    int _columns = 0;
    int _rows = 0;

    // scale appliqué au rendu
    float _scale = 1.f;

    Vector2 _pos;

public:
    AnimatedSprite(void) = default;

    int getFrameWidth(void) const { return _frameWidth; }
    int getFrameHeight(void) const { return _frameHeight; }
    bool isSheet(void) const { return _isSheet; }
    int getCurrentFrame(void) const { return _currentFrame; }

    std::vector<Texture2D *> getframes(void) const
    {
        return _frames;
    }

    void setScale(float f)
    {
        _scale = f;
    }

    Vector2 getPos(void) { return _pos; }
    void setPos(Vector2 pos) { _pos = pos; }

    // Charge une animation composée de plusieurs images séparées :
    // basePath0.png, basePath1.png, ...
    void load(GameContext &ctx, const std::string &basePath, int frameCount, float frameTime = 0.05f)
    {
        _frames.clear();
        _atlasFrames.clear();

        _isSheet = false;
        _frameTime = frameTime;

        for (int i = 0; i < frameCount; i++)
        {
            std::string path = basePath + std::to_string(i) + ".png";
            Texture2D &tex = ctx.loadTexture(path);
            _frames.push_back(&tex);
        }

        _ready = true;
    }

    // Charge une spritesheet organisée en grille
    void loadSheet(GameContext &ctx, const std::string &path, int columns, int rows, int frameCount, float frameTime = 0.05f)
    {
        _frames.clear();
        _atlasFrames.clear();

        _sheet = &ctx.loadTexture(path);

        if (!_sheet || _sheet->id == 0)
        {
            TraceLog(LOG_ERROR, "AnimatedSprite: failed to load %s", path.c_str());
            _ready = false;
            return;
        }

        _isSheet = true;
        _frameTime = frameTime;

        if (columns <= 0)
            columns = 1;
        if (rows <= 0)
            rows = 1;

        _columns = columns;
        _rows = rows;

        int maxFrames = columns * rows;

        // empêche de dépasser le nombre réel de frames disponibles
        if (frameCount <= 0 || frameCount > maxFrames)
            frameCount = maxFrames;

        _sheetFrameCount = frameCount;

        // calcul de la taille d'une frame dans la grille
        _frameWidth = _sheet->width / columns;
        _frameHeight = _sheet->height / rows;

        _currentFrame = 0;
        _timer = 0;

        _ready = true;
    }

    // Cas particulier : strip horizontal (1 ligne)
    void loadStrip(GameContext &ctx, const std::string &path, int frameCount, float frameTime = 0.05f)
    {
        loadSheet(ctx, path, frameCount, 1, frameCount, frameTime);
    }

    // Charge une spritesheet avec atlas JSON (TexturePacker, Aseprite export)
    void loadAtlas(GameContext &ctx, const std::string &texturePath, const std::string &jsonPath, float frameTime = 0.05f)
    {
        _frames.clear();
        _atlasFrames.clear();

        _sheet = &ctx.loadTexture(texturePath);

        if (!_sheet)
        {
            TraceLog(LOG_ERROR, "Atlas texture load failed");
            _ready = false;
            return;
        }

        std::ifstream file(jsonPath);
        if (!file.is_open())
        {
            TraceLog(LOG_ERROR, "Atlas JSON load failed");
            _ready = false;
            return;
        }

        nlohmann::json data;
        file >> data;

        // lecture des rectangles de frames dans le JSON
        for (auto &frame : data["frames"].items())
        {
            auto &f = frame.value()["frame"];

            Rectangle r;
            r.x = f["x"];
            r.y = f["y"];
            r.width = f["w"];
            r.height = f["h"];

            _atlasFrames.push_back(r);
        }

        _sheetFrameCount = _atlasFrames.size();

        _frameTime = frameTime;
        _isSheet = true;

        _currentFrame = 0;
        _timer = 0;

        _ready = true;
    }

    // Permet de charger des frames progressivement
    void loadFrame(GameContext &ctx, const std::string &basePath, int index)
    {
        std::string path = basePath + std::to_string(index) + ".png";
        Texture2D &tex = ctx.loadTexture(path);
        _frames.push_back(&tex);
    }

    // Termine le chargement progressif
    void finalize(float frameTime)
    {
        _frameTime = frameTime;
        _ready = true;
        _currentFrame = 0;
        _timer = 0.f;
    }

    // Avance l'animation selon le temps écoulé
    void update(float dt)
    {
        if (!_ready)
            return;

        int maxFrames = _isSheet ? _sheetFrameCount : _frames.size();

        if (maxFrames <= 1)
            return;

        _timer += dt;

        // permet de rattraper plusieurs frames si le dt est grand
        while (_timer >= _frameTime)
        {
            _timer -= _frameTime;

            _currentFrame++;

            if (_currentFrame >= maxFrames)
                _currentFrame = 0;
        }
    }

    // Dessine le sprite centré sur la position donnée
    void draw(Vector2 pos)
    {
        if (!_ready)
            return;

        if (_isSheet)
        {
            Rectangle src;

            // atlas JSON
            if (!_atlasFrames.empty())
                src = _atlasFrames[_currentFrame];
            else
            {
                // spritesheet en grille
                int col = _currentFrame % _columns;
                int row = _currentFrame / _columns;

                src = {
                    (float)(col * _frameWidth),
                    (float)(row * _frameHeight),
                    (float)_frameWidth,
                    (float)_frameHeight};
            }

            Rectangle dst{
                pos.x - src.width * _scale * 0.5f,
                pos.y - src.height * _scale * 0.5f,
                src.width * _scale,
                src.height * _scale};

            DrawTexturePro(*_sheet, src, dst, {0, 0}, 0, WHITE);
        }
        else
        {
            if (_frames.empty())
                return;

            Texture2D &tex = *_frames[_currentFrame];

            float w = tex.width * _scale;
            float h = tex.height * _scale;

            DrawTexturePro(tex,
                           {0, 0, (float)tex.width, (float)tex.height},
                           {pos.x - w * 0.5f, pos.y - h * 0.5f, w, h},
                           {0, 0},
                           0,
                           WHITE);
        }
    }

    // Dessine l'animation en plein écran
    void drawFullscreen(int width, int height)
    {
        if (!_ready || _frames.empty())
            return;

        Texture2D &tex = *_frames[_currentFrame];

        DrawTexturePro(tex,
                       {0, 0, (float)tex.width, (float)tex.height},
                       {0, 0, (float)width, (float)height},
                       {0, 0},
                       0,
                       WHITE);
    }

    // Dessin plein écran avec effet de parallaxe
    void drawFullscreen(int w, int h, float offsetX, float offsetY)
    {
        if (!_ready || _frames.empty())
            return;

        Texture2D &tex = *_frames[_currentFrame];

        float zoom = 1.1f;

        float drawW = w * zoom;
        float drawH = h * zoom;

        Rectangle src{0, 0, (float)tex.width, (float)tex.height};

        Rectangle dst{
            (w - drawW) * 0.5f + offsetX,
            (h - drawH) * 0.5f + offsetY,
            drawW,
            drawH};

        DrawTexturePro(tex, src, dst, {0, 0}, 0, WHITE);
    }
};