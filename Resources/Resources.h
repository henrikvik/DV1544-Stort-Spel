#pragma once
#include <map>
namespace Resources
{
    namespace Fonts
    {
        enum Files
        {
            KG14,
            KG18,
            KG26
        };
        const std::map<Files, const char *> Paths =
        {
            {KG14, R"(..\Resources\Fonts\KG14.spritefont)"},
            {KG18, R"(..\Resources\Fonts\KG18.spritefont)"},
            {KG26, R"(..\Resources\Fonts\KG26.spritefont)"}
        };
    }
    namespace Models
    {
        enum Files
        {
            Cube,
            SkySphere,
            Staff,
            Tablet1,
            UnitCube
        };
        const std::map<Files, const char *> Paths =
        {
            {Cube, R"(..\Resources\Models\Cube.hyb)"},
            {SkySphere, R"(..\Resources\Models\SkySphere.hyb)"},
            {Staff, R"(..\Resources\Models\Staff.hyb)"},
            {Tablet1, R"(..\Resources\Models\Tablet1.hyb)"},
            {UnitCube, R"(..\Resources\Models\UnitCube.hyb)"}
        };
    }
    namespace Shaders
    {
        enum Files
        {
            ForwardPlus,
            LightGridCulling,
            SkyShader,
            SpriteShader
        };
        const std::map<Files, const char *> Paths =
        {
            {ForwardPlus, R"(..\Resources\Shaders\ForwardPlus.hlsl)"},
            {LightGridCulling, R"(..\Resources\Shaders\LightGridCulling.hlsl)"},
            {SkyShader, R"(..\Resources\Shaders\SkyShader.hlsl)"},
            {SpriteShader, R"(..\Resources\Shaders\SpriteShader.hlsl)"}
        };
    }
    namespace Textures
    {
        enum Files
        {
            Backbutton,
            Color_pick_icons,
            crosshair,
            gameOverMenuButtons,
            gameOverTexture,
            HPBars,
            HUDIcons,
            mainMenuButton,
            mainmenupicture,
            MissingTexture,
            SettingsMenuButtons
        };
        const std::map<Files, const char *> Paths =
        {
            {Backbutton, R"(..\Resources\Textures\Backbutton.dds)"},
            {Color_pick_icons, R"(..\Resources\Textures\Color_pick_icons.dds)"},
            {crosshair, R"(..\Resources\Textures\crosshair.png)"},
            {gameOverMenuButtons, R"(..\Resources\Textures\gameOverMenuButtons.png)"},
            {gameOverTexture, R"(..\Resources\Textures\gameOverTexture.png)"},
            {HPBars, R"(..\Resources\Textures\HPBars.dds)"},
            {HUDIcons, R"(..\Resources\Textures\HUDIcons.dds)"},
            {mainMenuButton, R"(..\Resources\Textures\mainMenuButton.png)"},
            {mainmenupicture, R"(..\Resources\Textures\mainmenupicture.dds)"},
            {MissingTexture, R"(..\Resources\Textures\MissingTexture.png)"},
            {SettingsMenuButtons, R"(..\Resources\Textures\SettingsMenuButtons.png)"}
        };
    }
}
