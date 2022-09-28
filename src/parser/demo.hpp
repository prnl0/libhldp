#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "../utils/misc.hpp"
#include "../utils/bitbuffer.hpp"

#define DEMO_CONST(enum, member) utils::to_underlying(enum::constants_e::member)

struct demo
{
  /* All sizes listed as bytes. */
  enum class constants_e : std::uint16_t
  {
    header_size = 544,
    header_signature_check_size = 6,
    header_signature_size = 8,
    header_mapname_size = 260,
    header_gamedir_size = 260,

    min_dir_entry_count = 1,
    max_dir_entry_count = 1024,
    dir_entry_size = 92,
    dir_entry_description_size = 64
  };

  struct directory_entry
  {
    enum class type_e : std::uint32_t
    {
      loading = 0,
      playback,
      unknown
    };

    type_e type = type_e::unknown;
    std::string description;
    std::int32_t flags = 0;
    std::int32_t cdtrack = 0;
    float track_time = 0.0f;
    std::int32_t frames = 0;
    std::int32_t offset = 0;
    std::int32_t file_length = 0;
  };

  struct frame
  {
    enum class constants_e : std::uint16_t
    {
      min_seg_size = 12,
      seg_console_command_size = 64,
      seg_client_data_size = 32,
      seg_event_size = 84,
      seg_weapon_animation_size = 8,
      seg_sound_size_1 = 8,
      seg_sound_size_2 = 16,
      seg_demo_buffer_size = 4,
      seg_game_data_size = 468
    };

    enum class type_e : std::uint8_t
    {
      /* 0 and 1 -> game data */
      demo_start = 2, // no data
      console_command,
      client_data,
      next_section,   // no data
      event,
      weapon_anim,
      sound,
      demo_buffer
    };

    frame() = default;
    frame(const frame &f) : type(f.type), time(f.time), frame_no(f.frame_no)
    {
    }

    type_e type = type_e::demo_start;
    float time = 0.0f;
    std::uint32_t frame_no = 0;

    static const std::unordered_map<type_e, std::string> type_names;
  };

  struct console_command_frame : frame
  {
    console_command_frame(const frame &f) : frame(f) {}
    
    std::string command;
  };

  struct client_data_frame : frame
  {
    client_data_frame(const frame &f) : frame(f) {}

    float origin[3] = {0.0f};
    float viewangles[3] = {0.0f};
    std::int32_t wpn_bits = 0;
    float fov = 0.0f;
  };

  struct event_frame : frame
  {
    event_frame(const frame &f) : frame(f) {}

    std::int32_t flags = 0;
    std::int32_t idx = 0;
    float delay = 0.0f;

    struct
    {
      std::int32_t flags = 0;
      std::int32_t ent_idx = 0;
      float origin[3] = {0.0f};
      float angles[3] = {0.0f};
      float velocity[3] = {0.0f};
      std::int32_t ducking = 0;
      float fparams[2] = {0.0f};
      std::int32_t iparams[2] = {0};
      std::int32_t bparams[2] = {0};
    } args;
  };

  struct weapon_animation_frame : frame
  {
    weapon_animation_frame(const frame &f) : frame(f) {}

    std::int32_t anim = 0;
    std::int32_t body = 0;
  };

  struct sound_frame : frame
  {
    sound_frame(const frame &f) : frame(f) {}

    std::int32_t channel = 0;
    std::int32_t sample_size = 0;
    std::string sample;
    float attenuation = 0.0f;
    float volume = 0.0f;
    std::int32_t flags = 0;
    std::int32_t pitch = 0;
  };

  struct demo_buffer_frame : frame
  {
    demo_buffer_frame(const frame &f) : frame(f) {}

    std::int32_t buff_len = 0;
    std::string buff;
  };

  struct game_data_frame : frame
  {
    game_data_frame(const frame &f) : frame(f) {}

    enum class constants_e : std::uint32_t
    {
      demoinfo_size = 436,
      demoinfo_movevars_skyname_size = 32,
      min_message_length = 0,
      max_message_length = 65536
    };

    struct
    {
      float timestamp = 0.0f;

      struct
      {
        float vieworg[3] = {0.0f};
        float viewangles[3] = {0.0f};
        float forward[3] = {0.0f};
        float right[3] = {0.0f};
        float up[3] = {0.0f};
        float frame_time = 0.0f;
        float time = 0.0f;
        std::int32_t intermission = 0;
        std::int32_t paused = 0;
        std::int32_t spectator = 0;
        std::int32_t onground = 0;
        std::int32_t waterlevel = 0;
        float simvel[3] = {0.0f};
        float simorg[3] = {0.0f};
        float viewheight[3] = {0.0f};
        float ideal_pitch = 0.0f;
        float cl_viewangles[3] = {0.0f};
        std::int32_t health = 0;
        float crosshairangle[3] = {0.0f};
        float viewsize = 0;
        float punchangle[3] = {0.0f};
        std::int32_t max_clients = 0;
        std::int32_t viewentity = 0;
        std::int32_t playernum = 0;
        std::int32_t max_entities = 0;
        std::int32_t demo_playback = 0;
        std::int32_t hardware = 0;
        std::int32_t smoothing = 0;
        std::int32_t ptr_cmd = 0;
        std::int32_t ptr_movevars = 0;
        std::int32_t viewport[4] = {0};
        std::int32_t next_view = 0;
        std::int32_t only_client_draw = 0;
      } ref_params;

      struct
      {
        std::int16_t lerp_msec = 0;
        std::uint8_t msec = 0;
        std::uint8_t pad1 = 0;
        float viewangles[3] = {0.0f};
        float forwardmove = 0.0f;
        float sidemove = 0.0f;
        float upmove = 0.0f;
        std::int8_t lightlevel = 0;
        std::uint8_t pad2 = 0;
        std::uint16_t buttons = 0;
        std::int8_t impulse = 0;
        std::int8_t weapon_select = 0;
        std::uint8_t pad3[2] = {0};
        std::int32_t impact_idx = 0;
        float impact_pos[3] = {0.0f};
      } user_cmd;

      struct
      {
        float gravity = 0.0f;
        float stopspeed = 0.0f;
        float maxspeed = 0.0f;
        float spec_max_speed = 0.0f;
        float accelerate = 0.0f;
        float air_accelerate = 0.0f;
        float water_accelerate = 0.0f;
        float friction = 0.0f;
        float edge_friction = 0.0f;
        float water_friction = 0.0f;
        float ent_gravity = 0.0f;
        float bounce = 0.0f;
        float step_size = 0.0f;
        float max_velocity = 0.0f;
        float z_max = 0.0f;
        float wave_height = 0.0f;
        std::int32_t footsteps = 0;
        std::string sky_name = std::string(
          utils::to_underlying(constants_e::demoinfo_movevars_skyname_size), '\0'
        );
        float roll_angle = 0.0f;
        float roll_speed = 0.0f;
        float sky_color[3] = {0.0f};
        float sky_vec[3] = {0.0f};
      } move_vars;

      float view[3] = {0.0f};
      std::int32_t viewmodel = 0;
    } demo_info;

    std::int32_t inc_sequence = 0;
    std::int32_t inc_acknowledged = 0;
    std::int32_t inc_rel_acknowledged = 0;
    std::int32_t inc_rel_sequence = 0;
    std::int32_t out_sequence = 0;
    std::int32_t rel_sequence = 0;
    std::int32_t last_rel_sequence = 0;

    bit_buffer::data_t data;
  };

  std::int32_t dem_proto = 0;
  std::int32_t net_proto = 0;
  std::string game_dir;
  std::int32_t crc = 0;
  float duration = 0.0f;
  std::int32_t dir_offset = 0;
  std::vector<directory_entry> dir_entries;
};
