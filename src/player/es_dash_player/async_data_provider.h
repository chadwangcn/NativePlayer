/*!
 * async_data_provider.h (https://github.com/SamsungDForum/NativePlayer)
 * Copyright 2016, Samsung Electronics Co., Ltd
 * Licensed under the MIT license
 *
 * @author Tomasz Borkowski
 */

#ifndef NATIVE_PLAYER_SRC_PLAYER_ES_DASH_PLAYER_ASYNC_DATA_PROVIDER_H_
#define NATIVE_PLAYER_SRC_PLAYER_ES_DASH_PLAYER_ASYNC_DATA_PROVIDER_H_

#include <functional>
#include <memory>
#include <vector>

#include "ppapi/cpp/message_loop.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/utility/threading/lock.h"
#include "ppapi/utility/threading/simple_thread.h"

#include "dash/media_segment_sequence.h"

#include "media_segment.h"

class AsyncDataProvider {
 public:
  AsyncDataProvider(
      const pp::InstanceHandle& instance,
      std::function<void(std::unique_ptr<MediaSegment>)> callback);

  ~AsyncDataProvider() {}

  bool RequestNextDataSegment();

  bool SetNextSegmentToTime(double time);

  void SetMediaSegmentSequence(std::unique_ptr<MediaSegmentSequence> sequence);

  double AverageSegmentDuration();

  /// Needs to be called on non-main thread.
  bool GetInitSegment(std::vector<uint8_t>* buffer);

 private:
  void DownloadNextSegmentOnOwnThread(
      int32_t, MediaSegmentSequence::Iterator segment_iterator,
      pp::MessageLoop destination_message_loop);

  void PassResultOnCallerThread(int32_t, MediaSegment* segment);

  pp::SimpleThread own_thread_;
  std::unique_ptr<MediaSegmentSequence> sequence_;
  MediaSegmentSequence::Iterator next_segment_iterator_;

  pp::Lock iterator_lock_;
  pp::CompletionCallbackFactory<AsyncDataProvider> cc_factory_;
  size_t last_segment_size_;
  std::function<void(std::unique_ptr<MediaSegment>)> data_segment_callback_;
};

#endif  // NATIVE_PLAYER_SRC_PLAYER_ES_DASH_PLAYER_ASYNC_DATA_PROVIDER_H_
