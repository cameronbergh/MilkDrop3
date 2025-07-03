#ifndef MENU_STRINGS_H
#define MENU_STRINGS_H

#ifdef _WIN32
    // On Windows, use the existing resource system
    #include "resource.h" // For original IDS_ defines from .rc file
#else
    // On Linux, use a map-based lookup for essential strings
    #include <map>
    #include <string>
    #include <wchar.h>  // For wcscpy, wcsncpy if used
    #include <locale>   // For wstring_convert
    #include <codecvt>  // For wstring_convert

    // Define IDs that are used in menu.cpp and other places if not available
    // These should ideally match values from resource.h if there's any overlap
    // or be distinct if they are purely for Linux.
    // This is a minimal set; more would be needed for full functionality.
    #define IDS_UNTITLED_MENU_ITEM                          10001
    #define IDS_UNTITLED_MENU                               10002
    #define IDS_SZ_MENU_NAV_TOOLTIP                         10003 // "navigation: ESC: exit, Left Arrow: back, Right Arrow: select, UP/DOWN: change sel"
    #define IDS_ON                                          10004 // "ON"
    #define IDS_OFF                                         10005 // "OFF"
    #define IDS_MENU_LOCK_PRESET                            10006 // "Lock/Unlock Preset (L)" - Example, find real IDS if used
    #define IDS_MENU_RANDOM_PRESET                          10007 // "Random Preset (Spacebar)"
    #define IDS_MENU_NEXT_PRESET                            10008 // "Next Preset (Right Arrow)"
    #define IDS_MENU_PREV_PRESET                            10009 // "Previous Preset (Left Arrow)"
    #define IDS_MENU_EDIT_CUR_PRESET                        10010 // "Edit Current Preset (M)"
    #define IDS_MENU_SELECT_PRESET                          10011 // "Select Preset (L)"
    #define IDS_MENU_SHOW_PRESET_INFO                       10012 // "Show Preset Info (F4)"
    #define IDS_MENU_SHOW_FPS                               10013 // "Show FPS (F5)"
    #define IDS_MENU_SHOW_RATING                            10014 // "Show Rating (F6)"
	#define IDS_MENU_ALWAYS_ON_TOP							10015 // "Always On Top (F7)"
    // For CPlugin::MyRenderUI -> WaitString
    #define IDS_USE_UP_DOWN_ARROW_KEYS                      10016
    #define IDS_CURRENT_VALUE_OF_X                          10017
    #define IDS_ENTER_THE_NEW_STRING                        10018
    // For CPlugin::HandleRegularKey
    #define IDS_PRESET_ORDER_IS_NOW_X                       10019
    #define IDS_SEQUENTIAL                                  10020
    #define IDS_RANDOM                                      10021
    #define IDS_SHUFFLE_IS_NOW_ON                           10022
    #define IDS_COMPSHADER_LOCKED                           10023
    #define IDS_WARPSHADER_LOCKED                           10024
    #define IDS_ALLSHADERS_LOCKED                           10025
    #define IDS_ALLSHADERS_UNLOCKED                         10026
    // For CPlugin::AddError
    #define IDS_ERROR_NO_PRESET_FILE_FOUND_IN_X_MILK        10027
    // For CPlugin::LaunchSprite
    #define IDS_SPRITE_X_ERROR_COULD_NOT_FIND_IMG_OR_NOT_DEFINED 10028
    // For CPlugin::LoadPreset
    #define IDS_ERROR_PRESET_NOT_FOUND_X                    10029
    // For CPlugin::MyWindowProc -> case 'S'
    #define IDS_SAVE_AS                                     10030
    // For CPlugin::MyWindowProc -> case 'D' (delete)
    #define IDS_ARE_YOU_SURE_YOU_WANT_TO_DELETE_PRESET      10031
    #define IDS_PRESET_TO_DELETE                            10032
    // For CPlugin::MyWindowProc -> case UI_SAVE_OVERWRITE
    #define IDS_FILE_ALREADY_EXISTS_OVERWRITE_IT            10033
    #define IDS_FILE_IN_QUESTION_X_MILK                     10034
    #define IDS_WARNING_DO_NOT_FORGET_WARP_SHADER_WAS_LOCKED 10035
    #define IDS_WARNING_DO_NOT_FORGET_COMPOSITE_SHADER_WAS_LOCKED 10036
    // For CPlugin::SavePresetAs
    #define IDS_ERROR_UNABLE_TO_SAVE_THE_FILE               10037
    #define IDS_SAVE_SUCCESSFUL                             10038
    // For CPlugin::DeletePresetFile
    #define IDS_ERROR_UNABLE_TO_DELETE_THE_FILE             10039
    #define IDS_PRESET_X_DELETED                            10040
    // For CPlugin::RenamePresetFile
    #define IDS_ERROR_A_FILE_ALREADY_EXISTS_WITH_THAT_FILENAME 10041
    #define IDS_ERROR_UNABLE_TO_RENAME_FILE                 10042
    #define IDS_RENAME_SUCCESSFUL                           10043
    // For CPlugin::UpdatePresetList
    #define IDS_SCANNING_PRESETS                            10044
    // For CPlugin::AllocateMyDX9Stuff -> fallback shader errors
    #define IDS_FAILED_TO_COMPILE_PIXEL_SHADERS_USING_X     10045
    #define IDS_FAILED_TO_COMPILE_PIXEL_SHADERS_HARDWARE_MIS_REPORT 10046
    #define IDS_SHADER_MODEL_2                              10047
    #define IDS_SHADER_MODEL_3                              10048
    #define IDS_SHADER_MODEL_4                              10049 // Though likely not used
    #define IDS_UKNOWN_CASE_X                               10050
    #define IDS_MILKDROP_ERROR                              10051
    #define IDS_COULD_NOT_COMPILE_FALLBACK_WV_SHADER        10052
    #define IDS_COULD_NOT_COMPILE_FALLBACK_CV_SHADER        10053
    #define IDS_COULD_NOT_COMPILE_FALLBACK_CP_SHADER        10054
    #define IDS_COULD_NOT_COMPILE_BLUR1_VERTEX_SHADER       10055
    #define IDS_COULD_NOT_COMPILE_BLUR1_PIXEL_SHADER        10056
    #define IDS_COULD_NOT_COMPILE_BLUR2_VERTEX_SHADER       10057
    #define IDS_COULD_NOT_COMPILE_BLUR2_PIXEL_SHADER        10058
    #define IDS_COULD_NOT_CREATE_INTERNAL_CANVAS_TEXTURE_NOT_ENOUGH_VID_MEM 10059
    #define IDS_COULD_NOT_CREATE_INTERNAL_CANVAS_TEXTURE_NOT_ENOUGH_VID_MEM_RECOMMENDATION 10060
    #define IDS_SUCCESSFULLY_CREATED_VS0_VS1                10061
    #define IDS_ERROR_CREATING_BLUR_TEXTURES                10062
    #define IDS_MILKDROP_WARNING                            10063
    #define IDS_ERROR_CREATING_DOUBLE_SIZED_GDI_TITLE_FONT  10064
    #define IDS_ERROR_CREATING_DOUBLE_SIZED_D3DX_TITLE_FONT 10065
    // For CPlugin::RecompilePShader / LoadShaderFromMemory
    #define IDS_ERROR_COMPILING_X_X_SHADER                  10066
    #define IDS_ERROR_CREATING_SHADER                       10067
    // For CPlugin::AddNoiseTex/Vol
    #define IDS_COULD_NOT_CREATE_NOISE_TEXTURE              10068
    #define IDS_COULD_NOT_LOCK_NOISE_TEXTURE                10069
    #define IDS_NOISE_TEXTURE_BYTE_LAYOUT_NOT_RECOGNISED    10070
    #define IDS_COULD_NOT_CREATE_3D_NOISE_TEXTURE           10071
    #define IDS_COULD_NOT_LOCK_3D_NOISE_TEXTURE             10072
    #define IDS_3D_NOISE_TEXTURE_BYTE_LAYOUT_NOT_RECOGNISED 10073
    // For CPlugin::PickRandomTexture
    #define IDS_UNABLE_TO_READ_DATA_FILE_X                  10074
    // For CPlugin::HandleRegularKey
    #define IDS_ILLEGAL_CHARACTER                           10075
    #define IDS_STRING_TOO_LONG                             10076
    // For CPlugin::MyWindowProc -> UI_UPGRADE_PIXEL_SHADER
    #define IDS_PRESET_USES_HIGHEST_PIXEL_SHADER_VERSION    10077
    #define IDS_PRESS_ESC_TO_RETURN                         10078
    #define IDS_PRESET_HAS_MIXED_VERSIONS_OF_SHADERS        10079
    #define IDS_UPGRADE_SHADERS_TO_USE_PS2                  10080
    #define IDS_UPGRADE_SHADERS_TO_USE_PS2X                 10081
    #define IDS_UPGRADE_SHADERS_TO_USE_PS3                  10082
    #define IDS_PRESET_DOES_NOT_USE_PIXEL_SHADERS           10083
    #define IDS_WARNING_OLD_GPU_MIGHT_NOT_WORK_WITH_PRESET  10084
    #define IDS_PRESET_CURRENTLY_USES_PS2                   10085
    #define IDS_PRESET_CURRENTLY_USES_PS2X                  10086
    #define IDS_PRESET_CURRENTLY_USES_PS3                   10087
	#define IDS_UPGRADE_SHADERS_TO_USE_PS4					10088
    // For CPlugin::MyWindowProc -> UI_MASHUP
    #define IDS_PRESET_MASH_UP_TEXT1                        10089
    #define IDS_PRESET_MASH_UP_TEXT2                        10090
    #define IDS_PRESET_MASH_UP_TEXT3                        10091
    #define IDS_PRESET_MASH_UP_TEXT4                        10092
    #define IDS_MASHUP_GENERAL_POSTPROC                     10093
    #define IDS_MASHUP_MOTION_EQUATIONS                     10094
    #define IDS_MASHUP_WAVEFORMS_SHAPES                     10095
    #define IDS_MASHUP_WARP_SHADER                          10096
    #define IDS_MASHUP_COMP_SHADER                          10097
    // For CPlugin::MyWindowProc -> UI_LOAD
    #define IDS_LOAD_WHICH_PRESET_PLUS_COMMANDS             10098
    #define IDS_DIRECTORY_OF_X                              10099
    #define IDS_PARENT_DIRECTORY                            10100
    #define IDS_LOCKED                                      10101
    // For CPlugin::MyWindowProc -> case 'Y' (custom message)
    #define IDS_PAGE_X_OF_X                                 10102
    // For CPlugin::AllocateMyDX9Stuff
    #define IDS_COULD_NOT_CREATE_MY_VERTEX_DECLARATION      10103
    #define IDS_COULD_NOT_CREATE_WF_VERTEX_DECLARATION      10104
    #define IDS_COULD_NOT_CREATE_SPRITE_VERTEX_DECLARATION  10105
    // For menu items
    #define IDS_MENU_EDIT_PRESET_INIT_CODE_TT               10106
    #define IDS_MENU_EDIT_PER_FRAME_EQUATIONS_TT            10107
    #define IDS_MENU_EDIT_PER_VERTEX_EQUATIONS_TT           10108
    #define IDS_MENU_EDIT_WARP_SHADER_TT                    10109
    #define IDS_MENU_EDIT_COMPOSITE_SHADER_TT               10110
    #define IDS_MENU_EDIT_UPGRADE_PRESET_PS_VERSION_TT      10111
    #define IDS_MENU_EDIT_DO_A_PRESET_MASH_UP_TT            10112
    #define IDS_MENU_WAVE_TYPE_TT                           10113
    #define IDS_MENU_SIZE_TT                                10114
    #define IDS_MENU_SMOOTH_TT                              10115
    #define IDS_MENU_MYSTERY_PARAMETER_TT                   10116
    #define IDS_MENU_POSITION_X_TT                          10117
    #define IDS_MENU_POSITION_Y_TT                          10118
    #define IDS_MENU_COLOR_RED_TT                           10119
    #define IDS_MENU_COLOR_GREEN_TT                         10120
    #define IDS_MENU_COLOR_BLUE_TT                          10121
    #define IDS_MENU_OPACITY_TT                             10122
    #define IDS_MENU_USE_DOTS_TT                            10123
    #define IDS_MENU_DRAW_THICK_TT                          10124
    #define IDS_MENU_MODULATE_OPACITY_BY_VOLUME_TT          10125
    #define IDS_MENU_MODULATION_TRANSPARENT_VOLUME_TT       10126
    #define IDS_MENU_MODULATION_OPAQUE_VOLUME_TT            10127
    #define IDS_MENU_ADDITIVE_DRAWING_TT                    10128
    #define IDS_MENU_COLOR_BRIGHTENING_TT                   10129
    #define IDS_MENU_OUTER_BORDER_THICKNESS_TT              10130
    #define IDS_MENU_COLOR_RED_OUTER_TT                     10131
    #define IDS_MENU_COLOR_GREEN_OUTER_TT                   10132
    #define IDS_MENU_COLOR_BLUE_OUTER_TT                    10133
    #define IDS_MENU_OPACITY_OUTER_TT                       10134
    #define IDS_MENU_INNER_BORDER_THICKNESS_TT              10135
    #define IDS_MENU_COLOR_RED_INNER_TT                     10136
    #define IDS_MENU_COLOR_GREEN_INNER_TT                   10137
    #define IDS_MENU_COLOR_BLUE_INNER_TT                    10138
    #define IDS_MENU_OPACITY_INNER_TT                       10139
    #define IDS_MENU_MOTION_VECTOR_OPACITY_TT               10140
    #define IDS_MENU_NUM_MOT_VECTORS_X_TT                   10141
    #define IDS_MENU_NUM_MOT_VECTORS_Y_TT                   10142
    #define IDS_MENU_OFFSET_X_TT                            10143
    #define IDS_MENU_OFFSET_Y_TT                            10144
    #define IDS_MENU_TRAIL_LENGTH_TT                        10145
    #define IDS_MENU_COLOR_RED_MOTION_VECTOR_TT             10146
    #define IDS_MENU_COLOR_GREEN_MOTION_VECTOR_TT           10147
    #define IDS_MENU_COLOR_BLUE_MOTION_VECTOR_TT            10148
    #define IDS_MENU_ZOOM_AMOUNT_TT                         10149
    #define IDS_MENU_ZOOM_EXPONENT_TT                       10150
    #define IDS_MENU_WARP_AMOUNT_TT                         10151
    #define IDS_MENU_WARP_SCALE_TT                          10152
    #define IDS_MENU_WARP_SPEED_TT                          10153
    #define IDS_MENU_ROTATION_AMOUNT_TT                     10154
    #define IDS_MENU_ROTATION_CENTER_OF_X_TT                10155
    #define IDS_MENU_ROTATION_CENTER_OF_Y_TT                10156
    #define IDS_MENU_TRANSLATION_X_TT                       10157
    #define IDS_MENU_TRANSLATION_Y_TT                       10158
    #define IDS_MENU_SCALING_X_TT                           10159
    #define IDS_MENU_SCALING_Y_TT                           10160
    #define IDS_MENU_SUSTAIN_LEVEL_TT                       10161
    #define IDS_MENU_DARKEN_CENTER_TT                       10162
    #define IDS_MENU_GAMMA_ADJUSTMENT_TT                    10163
    #define IDS_MENU_HUE_SHADER_TT                          10164
    #define IDS_MENU_VIDEO_ECHO_ALPHA_TT                    10165
    #define IDS_MENU_VIDEO_ECHO_ZOOM_TT                     10166
    #define IDS_MENU_VIDEO_ECHO_ORIENTATION_TT              10167
    #define IDS_MENU_TEXTURE_WRAP_TT                        10168
    #define IDS_MENU_FILTER_INVERT_TT                       10169
    #define IDS_MENU_FILTER_BRIGHTEN_TT                     10170
    #define IDS_MENU_FILTER_DARKEN_TT                       10171
    #define IDS_MENU_FILTER_SOLARIZE_TT                     10172
    #define IDS_MENU_BLUR1_EDGE_DARKEN_AMOUNT_TT            10173
    #define IDS_MENU_BLUR1_MIN_COLOR_VALUE_TT               10174
    #define IDS_MENU_BLUR1_MAX_COLOR_VALUE_TT               10175
    #define IDS_MENU_BLUR2_MIN_COLOR_VALUE_TT               10176
    #define IDS_MENU_BLUR2_MAX_COLOR_VALUE_TT               10177
    #define IDS_MENU_BLUR3_MIN_COLOR_VALUE_TT               10178
    #define IDS_MENU_BLUR3_MAX_COLOR_VALUE_TT               10179
    #define IDS_MENU_ENABLED_TT                             10180
    #define IDS_MENU_NUMBER_OF_SAMPLES_TT                   10181
    #define IDS_MENU_L_R_SEPARATION_TT                      10182
    #define IDS_MENU_SCALING_TT                             10183
    #define IDS_MENU_SMOOTHING_TT                           10184
    #define IDS_MENU_OPACITY_WAVE_TT                        10185
    #define IDS_MENU_USE_SPECTRUM_TT                        10186
    #define IDS_MENU_USE_DOTS_WAVE_TT                       10187
    #define IDS_MENU_DRAW_THICK_WAVE_TT                     10188
    #define IDS_MENU_ADDITIVE_DRAWING_WAVE_TT               10189
    #define IDS_MENU_EXPORT_TO_FILE_TT                      10190
    #define IDS_MENU_IMPORT_FROM_FILE_TT                    10191
    #define IDS_MENU_EDIT_INIT_CODE_TT                      10192
    #define IDS_MENU_EDIT_PER_FRAME_CODE_TT                 10193
    #define IDS_MENU_EDIT_PER_POINT_CODE_TT                 10194
    #define IDS_MENU_ENABLED_SHAPE_TT                       10195
    #define IDS_MENU_NUMBER_OF_INSTANCES_TT                 10196
    #define IDS_MENU_NUMBER_OF_SIDES_TT                     10197
    #define IDS_MENU_DRAW_THICK_SHAPE_TT                    10198
    #define IDS_MENU_ADDITIVE_DRAWING_SHAPE_TT              10199
    #define IDS_MENU_X_POSITION_TT                          10200
    #define IDS_MENU_Y_POSITION_TT                          10201
    #define IDS_MENU_RADIUS_TT                              10202
    #define IDS_MENU_ANGLE_TT                               10203
    #define IDS_MENU_TEXTURED_TT                            10204
    #define IDS_MENU_TEXTURE_ZOOM_TT                        10205
    #define IDS_MENU_TEXTURE_ANGLE_TT                       10206
    #define IDS_MENU_INNER_COLOR_RED_TT                     10207
    #define IDS_MENU_INNER_COLOR_GREEN_TT                   10208
    #define IDS_MENU_INNER_COLOR_BLUE_TT                    10209
    #define IDS_MENU_INNER_OPACITY_TT                       10210
    #define IDS_MENU_OUTER_COLOR_RED_TT                     10211
    #define IDS_MENU_OUTER_COLOR_GREEN_TT                   10212
    #define IDS_MENU_OUTER_COLOR_BLUE_TT                    10213
    #define IDS_MENU_OUTER_OPACITY_TT                       10214
    #define IDS_MENU_BORDER_COLOR_RED_TT                    10215
    #define IDS_MENU_BORDER_COLOR_GREEN_TT                  10216
    #define IDS_MENU_BORDER_COLOR_BLUE_TT                   10217
    #define IDS_MENU_BORDER_OPACITY_TT                      10218
    #define IDS_MENU_EXPORT_TO_FILE_SHAPE_TT                10219
    #define IDS_MENU_IMPORT_FROM_FILE_SHAPE_TT              10220
    #define IDS_MENU_EDIT_INIT_CODE_SHAPE_TT                10221
    #define IDS_MENU_EDIT_PER_FRAME_INSTANCE_CODE_TT        10222
    // Utility.cpp strings
    #define IDS_URL_COULD_NOT_OPEN                          10223
    #define IDS_ACCESS_TO_URL_WAS_DENIED                    10224
    #define IDS_ACCESS_TO_URL_FAILED_DUE_TO_NO_ASSOC        10225
    #define IDS_ACCESS_TO_URL_FAILED_CODE_X                 10226
    #define IDS_ERROR_OPENING_URL                           10227
    #define IDS_DIRECTX_MISSING_OR_CORRUPT_TEXT             10228 // Long text, provide a shorter version
    #define IDS_DIRECTX_MISSING_OR_CORRUPT                  10229
    // Shell_defines.h (or where MyTextOut defines are used)
    #define IDS_EDIT_CURRENT_PRESET                         10230 // "Edit Current Preset"
    #define IDS_MOTION                                      10231
    #define IDS_DRAWING_CUSTOM_SHAPES                       10232
    #define IDS_DRAWING_CUSTOM_WAVES                        10233
    #define IDS_DRAWING_SIMPLE_WAVEFORM                     10234
    #define IDS_DRAWING_BORDERS_MOTION_VECTORS              10235
    #define IDS_POST_PROCESSING_MISC                        10236
    #define IDS_CUSTOM_WAVE_X                               10237
    #define IDS_CUSTOM_SHAPE_X                              10238
    #define IDS_MENU_EDIT_PRESET_INIT_CODE                  10239
    #define IDS_MENU_EDIT_PER_FRAME_EQUATIONS               10240
    #define IDS_MENU_EDIT_PER_VERTEX_EQUATIONS              10241
    #define IDS_MENU_EDIT_WARP_SHADER                       10242
    #define IDS_MENU_EDIT_COMPOSITE_SHADER                  10243
    #define IDS_MENU_EDIT_UPGRADE_PRESET_PS_VERSION         10244
    #define IDS_MENU_EDIT_DO_A_PRESET_MASH_UP               10245
    // ... more IDS_ defines as identified from menu.cpp and other files ...


    inline std::map<int, std::wstring>& get_string_table_instance() {
        static std::map<int, std::wstring> table;
        if (table.empty()) {
            // Populate with actual strings, this is just a subset
            table[IDS_UNTITLED_MENU_ITEM] = L"<untitled menu item>";
            table[IDS_UNTITLED_MENU]      = L"<untitled menu>";
            table[IDS_SZ_MENU_NAV_TOOLTIP]= L"navigation: ESC: exit, Left Arrow: back, Right Arrow: select, UP/DOWN: change sel";
            table[IDS_ON]                 = L"ON";
            table[IDS_OFF]                = L"OFF";
            table[IDS_MENU_LOCK_PRESET]   = L"Lock/Unlock Preset (Scroll Lock)";
            table[IDS_MENU_RANDOM_PRESET] = L"Random Preset (Spacebar)";
            // ... Add all other IDS_ defines here
            table[IDS_USE_UP_DOWN_ARROW_KEYS] = L"Use UP/DOWN arrow keys to adjust value, ENTER to accept, ESC to cancel.";
            table[IDS_CURRENT_VALUE_OF_X] = L"Current value of %ls:";
            table[IDS_ENTER_THE_NEW_STRING] = L"Enter the new string for %ls:";
            table[IDS_PRESET_ORDER_IS_NOW_X] = L"Preset order is now: %ls";
            table[IDS_SEQUENTIAL] = L"Sequential";
            table[IDS_RANDOM] = L"Random";
            table[IDS_SHUFFLE_IS_NOW_ON] = L"Shuffle is now ON";
            table[IDS_COMPSHADER_LOCKED] = L"Composite shader locked.";
            table[IDS_WARPSHADER_LOCKED] = L"Warp shader locked.";
            table[IDS_ALLSHADERS_LOCKED] = L"All shaders locked.";
            table[IDS_ALLSHADERS_UNLOCKED] = L"All shaders unlocked.";
            table[IDS_ERROR_NO_PRESET_FILE_FOUND_IN_X_MILK] = L"Error: No preset files (.milk) found in %ls";
            table[IDS_SPRITE_X_ERROR_COULD_NOT_FIND_IMG_OR_NOT_DEFINED] = L"Sprite #%d error: 'img=' line not found or image file not found.";
            table[IDS_ERROR_PRESET_NOT_FOUND_X] = L"Error: preset not found (%ls)";
            table[IDS_SAVE_AS] = L"Save Preset As:";
            table[IDS_ARE_YOU_SURE_YOU_WANT_TO_DELETE_PRESET] = L"Are you sure you want to delete this preset?";
            table[IDS_PRESET_TO_DELETE] = L"  Preset to delete: %ls";
            table[IDS_FILE_ALREADY_EXISTS_OVERWRITE_IT] = L"This file already exists. Overwrite it? (Y/N)";
            table[IDS_FILE_IN_QUESTION_X_MILK] = L"  File in question: %ls.milk";
            table[IDS_WARNING_DO_NOT_FORGET_WARP_SHADER_WAS_LOCKED] = L"WARNING: Don't forget - warp shader was locked!";
            table[IDS_WARNING_DO_NOT_FORGET_COMPOSITE_SHADER_WAS_LOCKED] = L"WARNING: Don't forget - composite shader was locked!";
            table[IDS_ERROR_UNABLE_TO_SAVE_THE_FILE] = L"Error: Unable to save the file.";
            table[IDS_SAVE_SUCCESSFUL] = L"Save successful.";
            table[IDS_ERROR_UNABLE_TO_DELETE_THE_FILE] = L"Error: Unable to delete the file.";
            table[IDS_PRESET_X_DELETED] = L"Preset '%ls' deleted.";
            table[IDS_ERROR_A_FILE_ALREADY_EXISTS_WITH_THAT_FILENAME] = L"Error: A file already exists with that filename.";
            table[IDS_ERROR_UNABLE_TO_RENAME_FILE] = L"Error: Unable to rename file.";
            table[IDS_RENAME_SUCCESSFUL] = L"Rename successful.";
            table[IDS_SCANNING_PRESETS] = L"Scanning presets...";
            table[IDS_FAILED_TO_COMPILE_PIXEL_SHADERS_USING_X] = L"MilkDrop failed to compile the pixel shaders using %ls (your video card reported that it should have been ableto, but it failed). Try updating your video card drivers. Specific error was: %ls";
            table[IDS_FAILED_TO_COMPILE_PIXEL_SHADERS_HARDWARE_MIS_REPORT] = L"MilkDrop failed to compile the pixel shaders (your video card might be mis-reporting its capabilities, or you might need to update your video card drivers). Specific error was: %ls";
            table[IDS_SHADER_MODEL_2] = L"Shader Model 2.0";
            table[IDS_SHADER_MODEL_3] = L"Shader Model 3.0";
            table[IDS_SHADER_MODEL_4] = L"Shader Model 4.0";
            table[IDS_UKNOWN_CASE_X] = L"unknown case: %d";
            table[IDS_MILKDROP_ERROR] = L"MilkDrop Error";
            table[IDS_URL_COULD_NOT_OPEN] = L"Could not open URL: %s";
            table[IDS_ACCESS_TO_URL_WAS_DENIED] = L"Access to URL was denied: %s";
            table[IDS_ACCESS_TO_URL_FAILED_DUE_TO_NO_ASSOC] = L"Access to URL failed due to no association: %s";
            table[IDS_ACCESS_TO_URL_FAILED_CODE_X] = L"Access to URL failed (code %d): %s";
            table[IDS_ERROR_OPENING_URL] = L"Error opening URL";
            table[IDS_DIRECTX_MISSING_OR_CORRUPT_TEXT] = L"DirectX initialisation failed (DirectX 9.0c or later is required). Try reinstalling DirectX and/or your video card drivers.\n\nWould you like to go to the DirectX download web page now?";
            table[IDS_DIRECTX_MISSING_OR_CORRUPT] = L"DirectX Missing or Corrupt";
            table[IDS_EDIT_CURRENT_PRESET] = L"Edit Current Preset (M)";
            table[IDS_MOTION] = L"Motion";
        }
        return table;
    }

    inline std::string WstringToUtf8String(const std::wstring& wstr) {
        try {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.to_bytes(wstr);
        } catch (const std::range_error&) {
            std::string dummy_str;
            for(wchar_t wc : wstr) {
                if (wc < 128 && wc > 0) dummy_str += static_cast<char>(wc); else dummy_str += '?';
            }
            return dummy_str;
        }
    }

    inline const wchar_t* LinuxLangStringW(int id, wchar_t* buffer = nullptr, int buffer_len = 0) {
        static std::wstring temp_wstring_storage;
        auto& table = get_string_table_instance();
        auto it = table.find(id);
        if (it != table.end()) {
            temp_wstring_storage = it->second;
            if (buffer && buffer_len > 0) {
               wcsncpy(buffer, temp_wstring_storage.c_str(), buffer_len -1);
               buffer[buffer_len-1] = L'\0';
               return buffer;
            }
            return temp_wstring_storage.c_str();
        }

        temp_wstring_storage = L"Unknown String ID: " + std::to_wstring(id);
        if (buffer && buffer_len > 0) {
            wcsncpy(buffer, temp_wstring_storage.c_str(), buffer_len-1);
            buffer[buffer_len-1] = L'\0';
            return buffer;
        }
        return temp_wstring_storage.c_str();
    }
    // Legacy: char* version, not used by menu.cpp but might be used elsewhere
    // For new Linux code, prefer using LinuxLangStringW and then WstringToUtf8String if char* is needed.
    /*
    inline const char* LinuxLangString(int id, char* buffer, int buffer_len) {
        static std::string temp_string_storage;
        std::wstring wstr = LinuxLangStringW(id);
        temp_string_storage = WstringToUtf8String(wstr);
        if (buffer && buffer_len > 0) {
            strncpy(buffer, temp_string_storage.c_str(), buffer_len -1);
            buffer[buffer_len-1] = '\0';
            return buffer;
        }
        return temp_string_storage.c_str();
    }
    */


#endif // _WIN32 / Linux
#endif // MENU_STRINGS_H
