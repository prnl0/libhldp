#include "parser.h"

#include <filesystem>
#include <cstdint>

#include "demo.h"

#include "../utils/misc.h"

parser::parser(const std::filesystem::path &demopath) : fdemo_(demopath)
{
  if (
    fdemo_.size() <
      utils::to_underlying(demo::constants::header_size) +
      static_cast<decltype(fdemo_.size())>(demo::constants::dir_entry_size) *
      utils::to_underlying(demo::constants::min_dir_entry_count)
  ) {
    throw parser_error(
      "invalid demo - size is less than (header_size + min_dir_entry_count * dir_entry_size)"
    );
  }

  if (fdemo_.read<std::string>() != "HLDEMO") {
    throw parser_error("invalid demo - bad header signature");
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
    .seek_bytes(utils::to_underlying(demo::constants::header_signature_size))
    .read(demo_.dem_proto)
    .read(demo_.net_proto);
  fdemo_.read_string(utils::to_underlying(demo::constants::header_mapname_size));
  fdemo_
    .read(demo_.game_dir, utils::to_underlying(demo::constants::header_gamedir_size))
    .read(demo_.crc)
    .read(demo_.dir_offset);
}

void parser::parse_directories()
{
  std::uint32_t dir_count = 0;
  fdemo_
    .seek_bytes(demo_.dir_offset)
    .read(dir_count);

  /* TODO: check for bogus amount of directories. */
  if (dir_count == 0) {
    throw parser_error(std::format("invalid number of directory entries ({})", dir_count));
  }

  for (decltype(dir_count) i = 0; i != dir_count; ++i) {
    demo::directory_entry e;
    fdemo_
      .read(e.type)
      .read(e.description, utils::to_underlying(demo::constants::dir_entry_description_size))
      .read(e.flags)
      .read(e.cdtrack)
      .read(e.track_time)
      .read(e.frames)
      .read(e.offset)
      .read(e.file_length);

    if (e.type == demo::directory_entry::type::playback) {
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
  //  if (!prelim_info_gathered_ && game_->local_player_) {
  //    basic_info_retrieved_ = true;
  //    return;
  //  }

    fdemo_.seek_bytes(e.offset);
    bool next_dir = false;
    while (!next_dir) {
      demo::frame df;
      fdemo_
        .read(df.type)
        .read(df.time)
        .read(df.frame);

  //    if (
  //      basic_info_retrieved_
  //      && entry.type == Demo::DirectoryEntry::Type::playback
  //      && valid_frames_
  //      ) {
  //      cur_frame_ = df.frame;

  //      /* Avoids duped frames. */
  //      if (df.frame >= frame_times_.size()) {
  //        if (cur_frame_ > 2) {
  //          fps_.push_back(1 / (df.time - frame_times_.back()));
  //        }
  //        frame_times_.push_back(df.time);
  //      }
  //    } else if (df.frame == 0) {
  //      valid_frames_ = true;
  //    }

      switch (df.type) {
        case demo::frame::Type::demo_start: {
          //store_frame(df, "demo_start");
          break;
        }

        case demo::frame::Type::console_command: {
          demo::console_command_frame ccf(df);
          fdemo_.read(ccf.command, demo::frame::seg_console_command_size);
          if (prelim_info_gathered_) {
            //config_.cl_cmds.push_back({cur_frame_, ccf.command});
          }
          //store_frame(ccf, "console_command");
          break;
        }

        case demo::frame::Type::client_data: {
          demo::client_data_frame cdf(df);
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

        case demo::frame::Type::next_section: {
          next_dir = true;
          //store_frame(df, "next_section");
          break;
        }

        case demo::frame::Type::event: {
          demo::event_frame ef(df);
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

        case demo::frame::Type::weapon_anim: {
          demo::weapon_animation_frame waf(df);
          fdemo_
            .read(waf.anim)
            .read(waf.body);
          //store_frame(waf, "weapon_animation");
          break;
        }

        case demo::frame::Type::sound: {
          demo::sound_frame sf(df);
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

        case demo::frame::Type::demo_buffer: {
          demo::demo_buffer_frame dbf(df);
          fdemo_
            .read(dbf.buff_len)
            .read(dbf.buff, dbf.buff_len);
          //store_frame(dbf, "demo_buffer");
          break;
        }

  //      /* Game data (types: 0, 1) */
  //      default:
  //      {
  //        GameDataFrame gdf(df);

  //        fb.read(gdf.demo_info.timestamp);

  //        fb.read(gdf.demo_info.ref_params.vieworg[0]);
  //        fb.read(gdf.demo_info.ref_params.vieworg[1]);
  //        fb.read(gdf.demo_info.ref_params.vieworg[2]);
  //        fb.read(gdf.demo_info.ref_params.viewangles[0]);
  //        fb.read(gdf.demo_info.ref_params.viewangles[1]);
  //        fb.read(gdf.demo_info.ref_params.viewangles[2]);
  //        fb.read(gdf.demo_info.ref_params.forward[0]);
  //        fb.read(gdf.demo_info.ref_params.forward[1]);
  //        fb.read(gdf.demo_info.ref_params.forward[2]);
  //        fb.read(gdf.demo_info.ref_params.right[0]);
  //        fb.read(gdf.demo_info.ref_params.right[1]);
  //        fb.read(gdf.demo_info.ref_params.right[2]);
  //        fb.read(gdf.demo_info.ref_params.up[0]);
  //        fb.read(gdf.demo_info.ref_params.up[1]);
  //        fb.read(gdf.demo_info.ref_params.up[2]);
  //        fb.read(gdf.demo_info.ref_params.frame_time);
  //        fb.read(gdf.demo_info.ref_params.time);
  //        fb.read(gdf.demo_info.ref_params.intermission);
  //        fb.read(gdf.demo_info.ref_params.paused);
  //        fb.read(gdf.demo_info.ref_params.spectator);
  //        fb.read(gdf.demo_info.ref_params.onground);
  //        fb.read(gdf.demo_info.ref_params.waterlevel);
  //        fb.read(gdf.demo_info.ref_params.simvel[0]);
  //        fb.read(gdf.demo_info.ref_params.simvel[1]);
  //        fb.read(gdf.demo_info.ref_params.simvel[2]);
  //        fb.read(gdf.demo_info.ref_params.simorg[0]);
  //        fb.read(gdf.demo_info.ref_params.simorg[1]);
  //        fb.read(gdf.demo_info.ref_params.simorg[2]);
  //        fb.read(gdf.demo_info.ref_params.viewheight[0]);
  //        fb.read(gdf.demo_info.ref_params.viewheight[1]);
  //        fb.read(gdf.demo_info.ref_params.viewheight[2]);
  //        fb.read(gdf.demo_info.ref_params.ideal_pitch);
  //        fb.read(gdf.demo_info.ref_params.cl_viewangles[0]);
  //        fb.read(gdf.demo_info.ref_params.cl_viewangles[1]);
  //        fb.read(gdf.demo_info.ref_params.cl_viewangles[2]);
  //        fb.read(gdf.demo_info.ref_params.health);
  //        fb.read(gdf.demo_info.ref_params.crosshairangle[0]);
  //        fb.read(gdf.demo_info.ref_params.crosshairangle[1]);
  //        fb.read(gdf.demo_info.ref_params.crosshairangle[2]);
  //        fb.read(gdf.demo_info.ref_params.viewsize);
  //        fb.read(gdf.demo_info.ref_params.punchangle[0]);
  //        fb.read(gdf.demo_info.ref_params.punchangle[1]);
  //        fb.read(gdf.demo_info.ref_params.punchangle[2]);
  //        fb.read(gdf.demo_info.ref_params.max_clients);
  //        fb.read(gdf.demo_info.ref_params.viewentity);
  //        fb.read(gdf.demo_info.ref_params.playernum);
  //        fb.read(gdf.demo_info.ref_params.max_entities);
  //        fb.read(gdf.demo_info.ref_params.demo_playback);
  //        fb.read(gdf.demo_info.ref_params.hardware);
  //        fb.read(gdf.demo_info.ref_params.smoothing);
  //        fb.read(gdf.demo_info.ref_params.ptr_cmd);
  //        fb.read(gdf.demo_info.ref_params.ptr_movevars);
  //        fb.read(gdf.demo_info.ref_params.viewport[0]);
  //        fb.read(gdf.demo_info.ref_params.viewport[1]);
  //        fb.read(gdf.demo_info.ref_params.viewport[2]);
  //        fb.read(gdf.demo_info.ref_params.viewport[3]);
  //        fb.read(gdf.demo_info.ref_params.next_view);
  //        fb.read(gdf.demo_info.ref_params.only_client_draw);

  //        fb.read(gdf.demo_info.user_cmd.lerp_msec);
  //        fb.read(gdf.demo_info.user_cmd.msec);
  //        fb.read(gdf.demo_info.user_cmd.pad1);
  //        fb.read(gdf.demo_info.user_cmd.viewangles[0]);
  //        fb.read(gdf.demo_info.user_cmd.viewangles[1]);
  //        fb.read(gdf.demo_info.user_cmd.viewangles[2]);
  //        fb.read(gdf.demo_info.user_cmd.forwardmove);
  //        fb.read(gdf.demo_info.user_cmd.sidemove);
  //        fb.read(gdf.demo_info.user_cmd.upmove);
  //        fb.read(gdf.demo_info.user_cmd.lightlevel);
  //        fb.read(gdf.demo_info.user_cmd.pad2);
  //        fb.read(gdf.demo_info.user_cmd.buttons);
  //        fb.read(gdf.demo_info.user_cmd.impulse);
  //        fb.read(gdf.demo_info.user_cmd.weapon_select);
  //        fb.read(gdf.demo_info.user_cmd.pad3[0]);
  //        fb.read(gdf.demo_info.user_cmd.pad3[1]);
  //        fb.read(gdf.demo_info.user_cmd.impact_idx);
  //        fb.read(gdf.demo_info.user_cmd.impact_pos[0]);
  //        fb.read(gdf.demo_info.user_cmd.impact_pos[1]);
  //        fb.read(gdf.demo_info.user_cmd.impact_pos[2]);

  //        fb.read(gdf.demo_info.move_vars.gravity);
  //        fb.read(gdf.demo_info.move_vars.stopspeed);
  //        fb.read(gdf.demo_info.move_vars.maxspeed);
  //        fb.read(gdf.demo_info.move_vars.spec_max_speed);
  //        fb.read(gdf.demo_info.move_vars.accelerate);
  //        fb.read(gdf.demo_info.move_vars.air_accelerate);
  //        fb.read(gdf.demo_info.move_vars.water_accelerate);
  //        fb.read(gdf.demo_info.move_vars.friction);
  //        fb.read(gdf.demo_info.move_vars.edge_friction);
  //        fb.read(gdf.demo_info.move_vars.water_friction);
  //        fb.read(gdf.demo_info.move_vars.ent_gravity);
  //        fb.read(gdf.demo_info.move_vars.bounce);
  //        fb.read(gdf.demo_info.move_vars.step_size);
  //        fb.read(gdf.demo_info.move_vars.max_velocity);
  //        fb.read(gdf.demo_info.move_vars.z_max);
  //        fb.read(gdf.demo_info.move_vars.wave_height);
  //        fb.read(gdf.demo_info.move_vars.footsteps);
  //        fb.read_string(
  //          gdf.demo_info.move_vars.sky_name,
  //          DemoFrame::seg_game_data_demoinfo_movevars_skyname_size
  //        );
  //        fb.read(gdf.demo_info.move_vars.roll_angle);
  //        fb.read(gdf.demo_info.move_vars.roll_speed);
  //        fb.read(gdf.demo_info.move_vars.sky_color[0]);
  //        fb.read(gdf.demo_info.move_vars.sky_color[1]);
  //        fb.read(gdf.demo_info.move_vars.sky_color[2]);
  //        fb.read(gdf.demo_info.move_vars.sky_vec[0]);
  //        fb.read(gdf.demo_info.move_vars.sky_vec[1]);
  //        fb.read(gdf.demo_info.move_vars.sky_vec[2]);

  //        fb.read(gdf.demo_info.view[0]);
  //        fb.read(gdf.demo_info.view[1]);
  //        fb.read(gdf.demo_info.view[2]);
  //        fb.read(gdf.demo_info.viewmodel);

  //        fb.read(gdf.inc_sequence);
  //        fb.read(gdf.inc_acknowledged);
  //        fb.read(gdf.inc_rel_acknowledged);
  //        fb.read(gdf.inc_rel_sequence);
  //        fb.read(gdf.out_sequence);
  //        fb.read(gdf.rel_sequence);
  //        fb.read(gdf.last_rel_sequence);

  //        std::uint32_t data_len = 0;
  //        fb.read(data_len);

  //        if (data_len != 0) {
  //          gdf.data = fb.read_bytes(data_len);
  //          parse_net_data(gdf.data);
  //        }

  //        if (prelim_info_gathered_ && lp) {
  //          lp->store(lp->health_, gdf.demo_info.ref_params.health * 1.0f);
  //        }

  //        store_frame(entry, gdf, "game_data");
  //        break;
  //      }
      }
    }
  }

  if (prelim_info_gathered_) {
  //  release_filebuffer();
  //  frame_times_.shrink_to_fit();
  //  parsed_ = true;
  }
}
