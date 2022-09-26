#include "parser.hpp"

#include <filesystem>
#include <cstdint>

#include "fmt/format.h"

#include "demo.hpp"

#include "../utils/misc.hpp"

parser::parser(const std::filesystem::path &demopath) : fdemo_(demopath)
{
  static constexpr auto min_size = DEMO_CONST(demo, header_size) +
    static_cast<decltype(fdemo_.size())>(DEMO_CONST(demo, dir_entry_size)) *
    DEMO_CONST(demo, min_dir_entry_count);
  if (fdemo_.size() < min_size) {
    throw parser_error(fmt::format(
      "demo size is less than (header_size + min_dir_entry_count * dir_entry_size " \
        "= {}B + {}B * {}B = {}B)",
      DEMO_CONST(demo, header_size),
      DEMO_CONST(demo, dir_entry_size),
      DEMO_CONST(demo, min_dir_entry_count),
      min_size
    ));
  }

  if (fdemo_.read<std::string>() != "HLDEMO") {
    throw parser_error("bad demo signature");
  }

  /* Parse some data without user input to retrieve preliminary information. */
  parse_header();
  parse_directories();
  parse_frames();
  fdemo_.release_data(); // release file data until there is further need of it
}

void parser::parse_header()
{
  fdemo_
    .seek_bytes(DEMO_CONST(demo, header_signature_size))
    .read(demo_.dem_proto)
    .read(demo_.net_proto);
  fdemo_.read_string(DEMO_CONST(demo, header_mapname_size));
  fdemo_
    .read(demo_.game_dir, DEMO_CONST(demo, header_gamedir_size))
    .read(demo_.crc)
    .read(demo_.dir_offset);
}

void parser::parse_directories()
{
  std::uint32_t dir_count = 0;
  fdemo_
    .seek_bytes(demo_.dir_offset)
    .read(dir_count);
  if (
    dir_count < DEMO_CONST(demo, min_dir_entry_count)
    || dir_count > DEMO_CONST(demo, max_dir_entry_count)
  ) {
    throw parser_error(fmt::format(
      "invalid number of directory entries (expected between {} and {}, got {})",
      DEMO_CONST(demo, min_dir_entry_count),
      DEMO_CONST(demo, max_dir_entry_count),
      dir_count
    ));
  }

  for (decltype(dir_count) i = 0; i != dir_count; ++i) {
    demo::directory_entry e;
    fdemo_
      .read(e.type)
      .read(e.description, DEMO_CONST(demo, dir_entry_description_size))
      .read(e.flags)
      .read(e.cdtrack)
      .read(e.track_time)
      .read(e.frames)
      .read(e.offset)
      .read(e.file_length);

    if (e.type == demo::directory_entry::type_e::playback) {
      demo_.duration = e.track_time;
      //frame_times_.reserve(
      //  /* TODO: usually occupies some frames less (up to 20-40k). */
      //  static_cast<decltype(frame_times_)::size_type>(entry.frames) * 5
      //);
    }

    demo_.dir_entries.push_back(std::move(e));
  }
}

void parser::parse_frames()
{
  /* Load demo into memory again, since we released it when parsing for
   * preliminary info. */
  if (!fdemo_.data_acquired()) {
    fdemo_.acquire_data();
  }

  //auto &lp = game_->local_player_;
  for (const auto &e : demo_.dir_entries) {
    //if (!prelim_info_gathered_ && game_->local_player_) {
    //  prelim_info_gathered_ = true;
    //  return;
    //}

    fdemo_.seek_bytes(e.offset);
    bool next_dir = false;
    while (!next_dir) {
      demo::frame frame;
      fdemo_
        .read(frame.type)
        .read(frame.time)
        .read(frame.frame_no);

      //if (
      //  basic_info_retrieved_
      //  && entry.type == Demo::DirectoryEntry::type_e::playback
      //  && valid_frames_
      //  ) {
      //  cur_frame_ = frame.frame;

      //  /* Avoids duped frames. */
      //  if (frame.frame >= frame_times_.size()) {
      //    if (cur_frame_ > 2) {
      //      fps_.push_back(1 / (frame.time - frame_times_.back()));
      //    }
      //    frame_times_.push_back(frame.time);
      //  }
      //} else if (frame.frame == 0) {
      //  valid_frames_ = true;
      //}

      switch (frame.type) {
        case demo::frame::type_e::demo_start: {
          //store_frame(frame, "demo_start");
          break;
        }

        case demo::frame::type_e::console_command: {
          demo::console_command_frame ccf(frame);
          fdemo_.read(ccf.command, DEMO_CONST(demo::frame, seg_console_command_size));
          //if (prelim_info_gathered_) {
          //  config_.cl_cmds.push_back({cur_frame_, ccf.command});
          //}
          //store_frame(ccf, "console_command");
          break;
        }

        case demo::frame::type_e::client_data: {
          demo::client_data_frame cdf(frame);
          fdemo_
            .read(cdf.origin[0])
            .read(cdf.origin[1])
            .read(cdf.origin[2])
            .read(cdf.viewangles[0])
            .read(cdf.viewangles[1])
            .read(cdf.viewangles[2])
            .read(cdf.wpn_bits)
            .read(cdf.fov);

          //if (prelim_info_gathered_ && lp) {
          //  lp->store(lp->position_[0], cdf.origin[0]);
          //  lp->store(lp->position_[1], cdf.origin[1]);
          //  lp->store(lp->position_[2], cdf.origin[2]);
          //  lp->store(lp->viewangles_[0], cdf.viewangles[0]);
          //  lp->store(lp->viewangles_[1], cdf.viewangles[1]);
          //  lp->store(lp->viewangles_[2], cdf.viewangles[2]);
          //  lp->store(lp->last_update_, cur_frame_);

          //  if (cdf.origin[0] < game_->map_.bounds_.x1) {
          //    game_->map_.bounds_.x1 = cdf.origin[0];
          //  } else if (cdf.origin[0] > game_->map_.bounds_.x2) {
          //    game_->map_.bounds_.x2 = cdf.origin[0];
          //  }

          //  if (cdf.origin[1] < game_->map_.bounds_.y1) {
          //    game_->map_.bounds_.y1 = cdf.origin[1];
          //  } else if (cdf.origin[1] > game_->map_.bounds_.y2) {
          //    game_->map_.bounds_.y2 = cdf.origin[1];
          //  }
          //}

          //store_frame(cdf, "client_data");
          break;
        }

        case demo::frame::type_e::next_section: {
          next_dir = true;
          //store_frame(frame, "next_section");
          break;
        }

        case demo::frame::type_e::event: {
          demo::event_frame ef(frame);
          fdemo_
            .read(ef.flags)
            .read(ef.idx)
            .read(ef.delay)
            .read(ef.args.flags)
            .read(ef.args.ent_idx)
            .read(ef.args.origin[0])
            .read(ef.args.origin[1])
            .read(ef.args.origin[2])
            .read(ef.args.angles[0])
            .read(ef.args.angles[1])
            .read(ef.args.angles[2])
            .read(ef.args.velocity[0])
            .read(ef.args.velocity[1])
            .read(ef.args.velocity[2])
            .read(ef.args.ducking)
            .read(ef.args.fparams[0])
            .read(ef.args.fparams[1])
            .read(ef.args.iparams[0])
            .read(ef.args.iparams[1])
            .read(ef.args.bparams[0])
            .read(ef.args.bparams[1]);
          //store_frame(ef, "event");
          break;
        }

        case demo::frame::type_e::weapon_anim: {
          demo::weapon_animation_frame waf(frame);
          fdemo_
            .read(waf.anim)
            .read(waf.body);
          //store_frame(waf, "weapon_animation");
          break;
        }

        case demo::frame::type_e::sound: {
          demo::sound_frame sf(frame);
          fdemo_
            .read(sf.channel)
            .read(sf.sample_size)
            .read(sf.sample, sf.sample_size)
            .read(sf.attenuation)
            .read(sf.volume)
            .read(sf.flags)
            .read(sf.pitch);
          //store_frame(sf, "sound");
          break;
        }

        case demo::frame::type_e::demo_buffer: {
          demo::demo_buffer_frame dbf(frame);
          fdemo_
            .read(dbf.buff_len)
            .read(dbf.buff, dbf.buff_len);
          //store_frame(dbf, "demo_buffer");
          break;
        }

        /* Game data (types: 0, 1) */
        default: {
          demo::game_data_frame gdf(frame);

          fdemo_
            .read(gdf.demo_info.timestamp)

            .read(gdf.demo_info.ref_params.vieworg[0])
            .read(gdf.demo_info.ref_params.vieworg[1])
            .read(gdf.demo_info.ref_params.vieworg[2])
            .read(gdf.demo_info.ref_params.viewangles[0])
            .read(gdf.demo_info.ref_params.viewangles[1])
            .read(gdf.demo_info.ref_params.viewangles[2])
            .read(gdf.demo_info.ref_params.forward[0])
            .read(gdf.demo_info.ref_params.forward[1])
            .read(gdf.demo_info.ref_params.forward[2])
            .read(gdf.demo_info.ref_params.right[0])
            .read(gdf.demo_info.ref_params.right[1])
            .read(gdf.demo_info.ref_params.right[2])
            .read(gdf.demo_info.ref_params.up[0])
            .read(gdf.demo_info.ref_params.up[1])
            .read(gdf.demo_info.ref_params.up[2])
            .read(gdf.demo_info.ref_params.frame_time)
            .read(gdf.demo_info.ref_params.time)
            .read(gdf.demo_info.ref_params.intermission)
            .read(gdf.demo_info.ref_params.paused)
            .read(gdf.demo_info.ref_params.spectator)
            .read(gdf.demo_info.ref_params.onground)
            .read(gdf.demo_info.ref_params.waterlevel)
            .read(gdf.demo_info.ref_params.simvel[0])
            .read(gdf.demo_info.ref_params.simvel[1])
            .read(gdf.demo_info.ref_params.simvel[2])
            .read(gdf.demo_info.ref_params.simorg[0])
            .read(gdf.demo_info.ref_params.simorg[1])
            .read(gdf.demo_info.ref_params.simorg[2])
            .read(gdf.demo_info.ref_params.viewheight[0])
            .read(gdf.demo_info.ref_params.viewheight[1])
            .read(gdf.demo_info.ref_params.viewheight[2])
            .read(gdf.demo_info.ref_params.ideal_pitch)
            .read(gdf.demo_info.ref_params.cl_viewangles[0])
            .read(gdf.demo_info.ref_params.cl_viewangles[1])
            .read(gdf.demo_info.ref_params.cl_viewangles[2])
            .read(gdf.demo_info.ref_params.health)
            .read(gdf.demo_info.ref_params.crosshairangle[0])
            .read(gdf.demo_info.ref_params.crosshairangle[1])
            .read(gdf.demo_info.ref_params.crosshairangle[2])
            .read(gdf.demo_info.ref_params.viewsize)
            .read(gdf.demo_info.ref_params.punchangle[0])
            .read(gdf.demo_info.ref_params.punchangle[1])
            .read(gdf.demo_info.ref_params.punchangle[2])
            .read(gdf.demo_info.ref_params.max_clients)
            .read(gdf.demo_info.ref_params.viewentity)
            .read(gdf.demo_info.ref_params.playernum)
            .read(gdf.demo_info.ref_params.max_entities)
            .read(gdf.demo_info.ref_params.demo_playback)
            .read(gdf.demo_info.ref_params.hardware)
            .read(gdf.demo_info.ref_params.smoothing)
            .read(gdf.demo_info.ref_params.ptr_cmd)
            .read(gdf.demo_info.ref_params.ptr_movevars)
            .read(gdf.demo_info.ref_params.viewport[0])
            .read(gdf.demo_info.ref_params.viewport[1])
            .read(gdf.demo_info.ref_params.viewport[2])
            .read(gdf.demo_info.ref_params.viewport[3])
            .read(gdf.demo_info.ref_params.next_view)
            .read(gdf.demo_info.ref_params.only_client_draw)

            .read(gdf.demo_info.user_cmd.lerp_msec)
            .read(gdf.demo_info.user_cmd.msec)
            .read(gdf.demo_info.user_cmd.pad1)
            .read(gdf.demo_info.user_cmd.viewangles[0])
            .read(gdf.demo_info.user_cmd.viewangles[1])
            .read(gdf.demo_info.user_cmd.viewangles[2])
            .read(gdf.demo_info.user_cmd.forwardmove)
            .read(gdf.demo_info.user_cmd.sidemove)
            .read(gdf.demo_info.user_cmd.upmove)
            .read(gdf.demo_info.user_cmd.lightlevel)
            .read(gdf.demo_info.user_cmd.pad2)
            .read(gdf.demo_info.user_cmd.buttons)
            .read(gdf.demo_info.user_cmd.impulse)
            .read(gdf.demo_info.user_cmd.weapon_select)
            .read(gdf.demo_info.user_cmd.pad3[0])
            .read(gdf.demo_info.user_cmd.pad3[1])
            .read(gdf.demo_info.user_cmd.impact_idx)
            .read(gdf.demo_info.user_cmd.impact_pos[0])
            .read(gdf.demo_info.user_cmd.impact_pos[1])
            .read(gdf.demo_info.user_cmd.impact_pos[2])

            .read(gdf.demo_info.move_vars.gravity)
            .read(gdf.demo_info.move_vars.stopspeed)
            .read(gdf.demo_info.move_vars.maxspeed)
            .read(gdf.demo_info.move_vars.spec_max_speed)
            .read(gdf.demo_info.move_vars.accelerate)
            .read(gdf.demo_info.move_vars.air_accelerate)
            .read(gdf.demo_info.move_vars.water_accelerate)
            .read(gdf.demo_info.move_vars.friction)
            .read(gdf.demo_info.move_vars.edge_friction)
            .read(gdf.demo_info.move_vars.water_friction)
            .read(gdf.demo_info.move_vars.ent_gravity)
            .read(gdf.demo_info.move_vars.bounce)
            .read(gdf.demo_info.move_vars.step_size)
            .read(gdf.demo_info.move_vars.max_velocity)
            .read(gdf.demo_info.move_vars.z_max)
            .read(gdf.demo_info.move_vars.wave_height)
            .read(gdf.demo_info.move_vars.footsteps)
            .read(
              gdf.demo_info.move_vars.sky_name,
              DEMO_CONST(demo::game_data_frame, demoinfo_movevars_skyname_size)
            )
            .read(gdf.demo_info.move_vars.roll_angle)
            .read(gdf.demo_info.move_vars.roll_speed)
            .read(gdf.demo_info.move_vars.sky_color[0])
            .read(gdf.demo_info.move_vars.sky_color[1])
            .read(gdf.demo_info.move_vars.sky_color[2])
            .read(gdf.demo_info.move_vars.sky_vec[0])
            .read(gdf.demo_info.move_vars.sky_vec[1])
            .read(gdf.demo_info.move_vars.sky_vec[2])

            .read(gdf.demo_info.view[0])
            .read(gdf.demo_info.view[1])
            .read(gdf.demo_info.view[2])
            .read(gdf.demo_info.viewmodel)

            .read(gdf.inc_sequence)
            .read(gdf.inc_acknowledged)
            .read(gdf.inc_rel_acknowledged)
            .read(gdf.inc_rel_sequence)
            .read(gdf.out_sequence)
            .read(gdf.rel_sequence)
            .read(gdf.last_rel_sequence);

          std::uint32_t data_len = 0;
          fdemo_.read(data_len);

          if (data_len != 0) {
            //gdf.data = fdemo_.read_bytes(data_len);
            //parse_net_data(gdf.data);
          }

          //if (prelim_info_gathered_ && lp) {
          //  lp->store(lp->health_, gdf.demo_info.ref_params.health * 1.0f);
          //}

          //store_frame(entry, gdf, "game_data");
          break;
        }
      }
    }
  }

  if (prelim_info_gathered_) {
    //release_filebuffer();
    //frame_times_.shrink_to_fit();
    //parsed_ = true;
  }
}
