#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/canonical_errors.h"
#include <iostream>
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/location_data.pb.h"
#include "mediapipe/framework/formats/wrapper_hand_tracking.pb.h"
#include <google/protobuf/util/json_util.h>
#include "mediapipe/framework/packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     8080
#define MAXLINE 1024
int sockfd;
struct sockaddr_in     servaddr;

namespace mediapipe {

constexpr char kLandmarksTag[] = "LANDMARKS";
constexpr char kNormLandmarksTag[] = "NORM_LANDMARKS"; // MAD note @to-fix: streaming NORM_LANDMARKS, but they're labeled LANDMARKS
constexpr char kNormRectTag[] = "NORM_RECT";
constexpr char kDetectionsTag[] = "DETECTIONS";
constexpr char recognizedHandGestureTag[] = "RECOGNIZED_HAND_GESTURE";

void setup_udp(){
	// int sockfd;
	char buffer[MAXLINE];
	// For testing the server
	// struct sockaddr_in servaddr;
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	
}

// A Calculator that simply passes its input Packets and header through,
// unchanged.  The inputs may be specified by tag or index.  The outputs
// must match the inputs exactly.  Any number of input side packets may
// also be specified.  If output side packets are specified, they must
// match the input side packets exactly and the Calculator passes its
// input side packets through, unchanged.  Otherwise, the input side
// packets will be ignored (allowing PassThroughCalculator to be used to
// test internal behavior).  Any options may be specified and will be
// ignored.
class JSONPassThroughCalculator : public CalculatorBase {
 public:
  static ::mediapipe::Status GetContract(CalculatorContract* cc) {
    if (!cc->Inputs().TagMap()->SameAs(*cc->Outputs().TagMap())) {
      return ::mediapipe::InvalidArgumentError(
          "Input and output streams to PassThroughCalculator must use "
          "matching tags and indexes.");
    }
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      cc->Inputs().Get(id).SetAny();
      cc->Outputs().Get(id).SetSameAs(&cc->Inputs().Get(id));
    }
    for (CollectionItemId id = cc->InputSidePackets().BeginId();
         id < cc->InputSidePackets().EndId(); ++id) {
      cc->InputSidePackets().Get(id).SetAny();
    }
    if (cc->OutputSidePackets().NumEntries() != 0) {
      if (!cc->InputSidePackets().TagMap()->SameAs(
              *cc->OutputSidePackets().TagMap())) {
        return ::mediapipe::InvalidArgumentError(
            "Input and output side packets to PassThroughCalculator must use "
            "matching tags and indexes.");
      }
      for (CollectionItemId id = cc->InputSidePackets().BeginId();
           id < cc->InputSidePackets().EndId(); ++id) {
        cc->OutputSidePackets().Get(id).SetSameAs(
            &cc->InputSidePackets().Get(id));
      }
    }
    return ::mediapipe::OkStatus();
  }

  ::mediapipe::Status Open(CalculatorContext* cc) final {
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      if (!cc->Inputs().Get(id).Header().IsEmpty()) {
        cc->Outputs().Get(id).SetHeader(cc->Inputs().Get(id).Header());
      }
    }
    if (cc->OutputSidePackets().NumEntries() != 0) {
      for (CollectionItemId id = cc->InputSidePackets().BeginId();
           id < cc->InputSidePackets().EndId(); ++id) {
        cc->OutputSidePackets().Get(id).Set(cc->InputSidePackets().Get(id));
      }
    }
    cc->SetOffset(TimestampDiff(0));

		//Socket
		setup_udp();

    return ::mediapipe::OkStatus();
  }

  ::mediapipe::Status Process(CalculatorContext* cc) final {
    cc->GetCounter("PassThrough")->Increment();
    if (cc->Inputs().NumEntries() == 0) {
      return tool::StatusStop();
    }
    for (CollectionItemId id = cc->Inputs().BeginId();
         id < cc->Inputs().EndId(); ++id) {
      if (!cc->Inputs().Get(id).IsEmpty()) {


        /*-------------------------------------------------------------------*/
        /*------------ EDITS to original pass_through_calculator ------------*/
        /*-------------------------------------------------------------------*/

        WrapperHandTracking* wrapper = new WrapperHandTracking();
        wrapper->InitAsDefaultInstance();

        if (cc->Inputs().Get(id).Name() == "hand_landmarks"){
          // the type is a NormalizedLandmarkList, but you need the kLandmarksTag
          // in order for it not to crash for some reason ...
          const NormalizedLandmarkList& landmarks = cc->Inputs().Tag(kLandmarksTag).Get<NormalizedLandmarkList>();


          for (int i = 0; i < landmarks.landmark_size(); ++i) {
              const NormalizedLandmark& landmark = landmarks.landmark(i);
              // std::cout << "Landmark " << i <<":\n" << landmark.DebugString() << '\n';

              wrapper->mutable_landmarks()->add_landmark();
              int size = wrapper->mutable_landmarks()->landmark_size()-1;
              wrapper->mutable_landmarks()->mutable_landmark(size)->set_x(landmark.x());
              wrapper->mutable_landmarks()->mutable_landmark(size)->set_y(landmark.y());
              wrapper->mutable_landmarks()->mutable_landmark(size)->set_z(landmark.z());
          }
        }

        if (cc->Inputs().Get(id).Name() == "palm_detections"){
          // Palm is detected once, not continuously — when it first shows up in the image
          const auto& detections = cc->Inputs().Tag(kDetectionsTag).Get<std::vector<Detection>>();
          for (int i = 0; i < detections.size(); ++i) {
              const Detection& detection = detections[i];
              // std::cout << "\n----- Detection -----\n " << detection.DebugString() << '\n';
              // wrapper->mutable_detection()->add_detection();
          //Just wanted to test if it will be converted.. It worked
					//std::string msg_buffer;
					//For JSON conversion
					//proto_ns::util::JsonPrintOptions options;
					//proto_ns::util::MessageToJsonString(detection, &msg_buffer, options);
					//For testing JSON string
         	//std::cout << "JSON Palm: " << msg_buffer << '\n';

          }
        }

        if (cc->Inputs().Get(id).Name() == "hand_rect"){
          // The Hand Rect is an x,y center, width, height, and angle (in radians)
          const NormalizedRect& rect = cc->Inputs().Tag(kNormRectTag).Get<NormalizedRect>();

          wrapper->mutable_rect()->set_x_center(rect.x_center());
          wrapper->mutable_rect()->set_y_center(rect.y_center());
          wrapper->mutable_rect()->set_width(rect.width());
          wrapper->mutable_rect()->set_height(rect.height());
          wrapper->mutable_rect()->set_rotation(rect.rotation());

          std::string msg_buffer;
					//For JSON conversion
					proto_ns::util::JsonPrintOptions options;
					proto_ns::util::MessageToJsonString(rect, &msg_buffer, options);
					//For testing JSON string
         	//std::cout << "JSON Hand Rect: " << msg_buffer << '\n';

					//Socket
					//sendto(sockfd, msg_buffer.c_str(), msg_buffer.length(),
            //  0, (const struct sockaddr *) &servaddr,
              //    sizeof(servaddr));

        }

				// For gesture
				if (cc->Inputs().Get(id).Name() == "recognized_gesture"){
           const auto& gesture = cc->Inputs().Tag(recognizedHandGestureTag).Get<std::string>();
                std::cout << "\n----- Gesture -----\n " << gesture << '\n';
           //std::string msg_buffer;
           //For JSON conversion
           //proto_ns::util::JsonPrintOptions options;
           //proto_ns::util::MessageToJsonString(detection, &msg_buffer, options);
           //For testing JSON string
           //std::cout << "JSON Palm: " << msg_buffer << '\n';
         }




        std::string msg_buffer;
				proto_ns::MessageLite* ConvertToMessageLite(wrapper);
				//For JSON conversion
				proto_ns::util::JsonPrintOptions options;
				proto_ns::util::MessageToJsonString(*wrapper, &msg_buffer, options);
				//For testing JSON string
        std::cout << "JSON Wrapper: " << msg_buffer << '\n';

				//Socket
				sendto(sockfd, msg_buffer.c_str(), msg_buffer.length(),
            0, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));

      /*-------------------------------------------------------------------*/
        VLOG(3) << "Passing " << cc->Inputs().Get(id).Name() << " to "
                << cc->Outputs().Get(id).Name() << " at "
                << cc->InputTimestamp().DebugString();
        cc->Outputs().Get(id).AddPacket(cc->Inputs().Get(id).Value());
      }
    }
    return ::mediapipe::OkStatus();
  }

  ::mediapipe::Status Close(CalculatorContext* cc) {
    if (!cc->GraphStatus().ok()) {
      return ::mediapipe::OkStatus();
    }
    // Socket
		close(sockfd);
    return ::mediapipe::OkStatus();
  }

};

REGISTER_CALCULATOR(JSONPassThroughCalculator);

}  // namespace mediapipe
