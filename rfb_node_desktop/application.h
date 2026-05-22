//
// Created by camilo on 2026-02-12 01:59 <3ThomasBorregaardSørensen!!
//

#pragma once


#include "apex/platform/application.h"



namespace remoting_rfb_node_desktop
{


   //class remoting;

   class CLASS_DECL_REMOTING_NODE_DESKTOP application :
      virtual public ::apex::application
   {
   public:

      bool m_bOpenFile = false;

      ::pointer < Server > m_pserver;
      ::pointer<Service> m_pservice;
      ::pointer<ServerApplication> m_pserverapplication;

      ///::pointer<remoting> m_premoting;

      __DECLARE_APPLICATION_RELEASE_TIME();

      application();
      ~application() override;
      
      
      void init_instance() override;

      void on_request(::request * prequest) override;

      bool check_pipe_node_client_executable_paths(const file::path& pathNode, const file::path& pathClient) override;


      ::lresult handle_direct_id(const ::enum_id eid, ::wparam wparam, ::lparam lparam) override;

      Server & Server();

      virtual void main_node(const ::file::path & path);


#include <iostream>
#include <rtc/rtc.hpp>
#include <thread>
#include <vector>

      using namespace rtc;

      // -----------------------------
      // Simple input protocol
      // -----------------------------
      enum InputType : uint8_t
      {
         MOUSE_MOVE = 1,
         MOUSE_CLICK = 2,
         KEY_EVENT = 3
      };

      struct InputMessage
      {
         uint8_t type;
         int32_t x;
         int32_t y;
         uint32_t keycode;
         uint8_t flags;
      };

      // -----------------------------
      // WebRTC Session
      // -----------------------------
      class WebRTCSession
      {
      public:

         PeerConnection pc;

         // Channels
         std::shared_ptr<DataChannel> inputChannel;

         // Media tracks
         std::shared_ptr<Track> videoTrack;
         std::shared_ptr<Track> audioTrack;

         WebRTCSession()
         {
            PeerConnection::Configuration config;

            // STUN (required for real internet use)
            config.iceServers.emplace_back("stun:stun.l.google.com:19302");

            pc = PeerConnection(config);

            setupEvents();
            setupChannels();
         }

         // -----------------------------
         // Setup DataChannel (INPUT)
         // -----------------------------
         void setupChannels()
         {
            DataChannelInit init;
            init.reliability = Reliability::Unreliable; // low latency

            inputChannel = pc.createDataChannel("input", init);

            inputChannel->onMessage(
               [](std::variant<binary, string> msg)
               {
                  if (std::holds_alternative<binary>(msg))
                  {
                     auto &data = std::get<binary>(msg);

                     if (data.size() == sizeof(InputMessage))
                     {
                        InputMessage im;
                        memcpy(&im, data.data(), sizeof(im));

                        handleInput(im);
                     }
                  }
               });
         }

         // -----------------------------
         // Setup event handlers
         // -----------------------------
         void setupEvents()
         {
            pc.onStateChange([](PeerConnection::State state) { std::cout << "State: " << (int)state << std::endl; });

            pc.onGatheringStateChange([](PeerConnection::GatheringState state)
                                      { std::cout << "ICE Gathering: " << (int)state << std::endl; });

            pc.onLocalDescription([](const Description &desc) { std::cout << "SDP OFFER:\n" << desc << std::endl; });

            pc.onLocalCandidate([](const Candidate &cand) { std::cout << "ICE candidate:\n" << cand << std::endl; });
         }

         // -----------------------------
         // VIDEO: send encoded H264
         // -----------------------------
         void sendVideoPacket(const uint8_t *data, size_t size)
         {
            if (!videoTrack)
               return;

            videoTrack->send(data, size);
         }

         // -----------------------------
         // AUDIO: send Opus frames
         // -----------------------------
         void sendAudioPacket(const uint8_t *data, size_t size)
         {
            if (!audioTrack)
               return;

            audioTrack->send(data, size);
         }

         // -----------------------------
         // INPUT handling (receiver side)
         // -----------------------------
         static void handleInput(const InputMessage &msg)
         {
            switch (msg.type)
            {
               case MOUSE_MOVE:
                  std::cout << "Mouse move: " << msg.x << ", " << msg.y << std::endl;
                  break;

               case MOUSE_CLICK:
                  std::cout << "Mouse click\n";
                  break;

               case KEY_EVENT:
                  std::cout << "Key: " << msg.keycode << std::endl;
                  break;
            }
         }

         // -----------------------------
         // Create media tracks
         // -----------------------------
         void addMediaTracks()
         {
            videoTrack = pc.addTrack("video");
            audioTrack = pc.addTrack("audio");

            videoTrack->onOpen([]() { std::cout << "Video track open\n"; });

            audioTrack->onOpen([]() { std::cout << "Audio track open\n"; });
         }

         // -----------------------------
         // Send input to remote side
         // -----------------------------
         void sendMouseMove(int x, int y)
         {
            InputMessage msg{};
            msg.type = MOUSE_MOVE;
            msg.x = x;
            msg.y = y;

            inputChannel->send(Binary(reinterpret_cast<const std::byte *>(&msg), sizeof(msg)));
         }

         void sendKey(uint32_t key, bool down)
         {
            InputMessage msg{};
            msg.type = KEY_EVENT;
            msg.keycode = key;
            msg.flags = down;

            inputChannel->send(Binary(reinterpret_cast<const std::byte *>(&msg), sizeof(msg)));
         }
      };

      // -----------------------------
      // Example usage
      // -----------------------------
      virtual int transmit_remoting();

   };


} // namespace remoting_rfb_node_desktop


