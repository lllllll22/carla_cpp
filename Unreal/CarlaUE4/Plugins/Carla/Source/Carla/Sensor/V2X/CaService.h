// Copyright (c) 2024 Institut fuer Technik der Informationsverarbeitung (ITIV) at the
// Karlsruhe Institute of Technology
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"
#include "carla/geom/Math.h"
#include "carla/geom/GeoLocation.h"
#include <vector>
#include "carla/sensor/data/LibITS.h"
#include <chrono>

class CaService
{
public:
// 构造函数，接收一个随机数引擎指针作为参数，可能用于生成噪声或其他随机化操作
  CaService(URandomEngine *random_engine);
  void SetOwner(UWorld *world, AActor *Owner);

  void SetParams(const float GenCamMin, const float GenCamMax, const bool FixedRate);
  void SetVelDeviation(const float noise_vel_stddev_x);
  void SetYawrateDeviation(const float noise_yawrate_stddev, const float noise_yawrate_bias);
  void SetAccelerationStandardDeviation(const FVector &Vec);
// 设置GNSS（全球导航卫星系统）位置和方向的偏差，包括纬度、经度、高度和航向的标准差及偏置
  void SetGNSSDeviation(const float noise_lat_stddev,
                        const float noise_lon_stddev,
                        const float noise_alt_stddev,
                        const float noise_head_stddev,
                        const float noise_lat_bias,
                        const float noise_lon_bias,
                        const float noise_alt_bias,
                        const float noise_head_bias);
// 触发一次服务更新，基于给定的时间间隔 DeltaSeconds。返回值可能是表示成功与否的布尔值
  bool Trigger(float DeltaSeconds);
// 获取当前生成的CAM（Cooperative Awareness Message，合作意识消息）信息
  // CAM_t 是一个自定义类型，代表要生成的消息格式
  CAM_t GetCamMessage();

private:
  AActor *mActorOwner;
  FCarlaActor *mCarlaActor;
  UCarlaEpisode *mCarlaEpisode;
  UWorld *mWorld;
  ACarlaWheeledVehicle *mVehicle;
  float mLastCamTimeStamp;
  float mLastLowCamTimeStamp;
  float mGenCamMin;
  float mGenCamMax;
  float mGenCam;
  double mElapsedTime;
  float mGenDeltaTimeMod;
  bool mDccRestriction;
  bool mFixedRate;
  unsigned int mGenCamLowDynamicsCounter;
  unsigned int mGenCamLowDynamicsLimit;
  float mGenerationInterval;

  float VehicleSpeed;
  FVector VehiclePosition;
  FVector VehicleHeading;

  FVector mLastCamPosition;
  float mLastCamSpeed;
  FVector mLastCamHeading;
  std::chrono::milliseconds mGenerationDelta0;

  bool CheckTriggeringConditions(float DeltaSeconds);
  bool CheckHeadingDelta(float DeltaSeconds);
  bool CheckPositionDelta(float DeltaSeconds);
  bool CheckSpeedDelta(float DeltaSeconds);
  double GetFVectorAngle(const FVector &a, const FVector &b);
  void GenerateCamMessage(float DeltaTime);
  ITSContainer::StationType_t GetStationType();

  float GetHeading();
  long GetVehicleRole();

  /* Constant values for message*/
  const long mProtocolVersion = 2;
  const long mMessageId = ITSContainer::messageID_cam;
  long mStationId;
  long mStationType;

  carla::geom::Vector3D ComputeAccelerometer(const float DeltaTime);
  const carla::geom::Vector3D ComputeAccelerometerNoise(const FVector &Accelerometer);
  /// Standard deviation for acceleration settings.
  FVector StdDevAccel;

  /// Used to compute the acceleration
  std::array<FVector, 2> PrevLocation;

  /// Used to compute the acceleration
  float PrevDeltaTime;

  // GNSS reference position and heading
  FVector GetReferencePosition();
  carla::geom::GeoLocation CurrentGeoReference;
  float LatitudeDeviation;
  float LongitudeDeviation;
  float AltitudeDeviation;
  float HeadingDeviation;

  float LatitudeBias;
  float LongitudeBias;
  float AltitudeBias;
  float HeadingBias;

  // Velocity
  float ComputeSpeed();
  float VelocityDeviation;

  // Yaw rate
  const float ComputeYawNoise(const FVector &Gyroscope);
  float ComputeYawRate();
  float YawrateDeviation;
  float YawrateBias;

  CAM_t CreateCooperativeAwarenessMessage(float DeltaTime);
  void CreateITSPduHeader(CAM_t &message);
  void AddCooperativeAwarenessMessage(CAMContainer::CoopAwareness_t &CoopAwarenessMessage, float DeltaTime);
  void AddBasicContainer(CAMContainer::BasicContainer_t &BasicContainer);
  void AddBasicVehicleContainerHighFrequency(CAMContainer::HighFrequencyContainer_t &hfc, float DeltaTime);
  void AddRSUContainerHighFrequency(CAMContainer::HighFrequencyContainer_t &hfc);
  void AddLowFrequencyContainer(CAMContainer::LowFrequencyContainer_t &lfc);
  CAM_t mCAMMessage;

  // random for noise
  URandomEngine *mRandomEngine;
  ITSContainer::SpeedValue_t BuildSpeedValue(const float vel);
};
