#include "config.h"
#include "common.h"
#include "helpers.h"
#include "logging.h"

#define MODEL_CHANGED bit(1)
#define VTX_CHANGED bit(2)
#define MAIN_CHANGED bit(3) // catch-all for global config item
#define FAN_CHANGED bit(4)
#define MOTION_CHANGED bit(5)
#define BUTTON_CHANGED bit(6)
#define ALL_CHANGED (MODEL_CHANGED | VTX_CHANGED | MAIN_CHANGED | FAN_CHANGED | MOTION_CHANGED | BUTTON_CHANGED)

// // Really awful but safe(?) type punning of model_config_t/v6_model_config_t to and from uint32_t
// template <class T>
// static const void U32_to_Model(uint32_t const u32, T *const model)
// {
//     union
//     {
//         union
//         {
//             T model;
//             uint8_t padding[sizeof(uint32_t) - sizeof(T)];
//         } val;
//         uint32_t u32;
//     } converter = {.u32 = u32};

//     *model = converter.val.model;
// }

// template <class T>
// static const uint32_t Model_to_U32(T const *const model)
// {
//     // clear the entire union because the assignment will only fill sizeof(T)
//     union
//     {
//         union
//         {
//             T model;
//             uint8_t padding[sizeof(uint32_t) - sizeof(T)];
//         } val;
//         uint32_t u32;
//     } converter = {0};

//     converter.val.model = *model;
//     return converter.u32;
// }

// TxConfig::TxConfig() : m_model(m_config.model_config)
// {
// }

// void TxConfig::Load()
// {
//     m_modified = 0;

//     // // Initialize NVS
//     // esp_err_t err = nvs_flash_init();
//     // if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     // {
//     //     ESP_ERROR_CHECK(nvs_flash_erase());
//     //     err = nvs_flash_init();
//     // }
//     // ESP_ERROR_CHECK( err );
//     // ESP_ERROR_CHECK(nvs_open("ELRS", NVS_READWRITE, &handle));

//     // // Try to load the version and make sure it is a TX config
//     // uint32_t version = 0;
//     // if (nvs_get_u32(handle, "tx_version", &version) == ESP_OK && ((version & CONFIG_MAGIC_MASK) == TX_CONFIG_MAGIC))
//     //     version = version & ~CONFIG_MAGIC_MASK;
//     // DBGLN("Config version %u", version);

//     // // Can't upgrade from version <5, or when flashing a previous version, just use defaults.
//     // if (version < 5 || version > TX_CONFIG_VERSION)
//     // {
//     //     SetDefaults(true);
//     //     return;
//     // }

//     // SetDefaults(false);

//     // uint32_t value;
//     // uint8_t value8;
//     // // vtx (v5)
//     // if (nvs_get_u32(handle, "vtx", &value) == ESP_OK)
//     // {
//     //     m_config.vtxBand = value >> 24;
//     //     m_config.vtxChannel = value >> 16;
//     //     m_config.vtxPower = value >> 8;
//     //     m_config.vtxPitmode = value;
//     // }

//     // // fanthresh (v5)
//     // if (nvs_get_u8(handle, "fanthresh", &value8) == ESP_OK)
//     //     m_config.powerFanThreshold = value8;

//     // // Both of these were added to config v5 without incrementing the version
//     // if (nvs_get_u32(handle, "fan", &value) == ESP_OK)
//     //     m_config.fanMode = value;
//     // if (nvs_get_u32(handle, "motion", &value) == ESP_OK)
//     //     m_config.motionMode = value;

//     // if (version >= 6)
//     // {
//     //     // dvr (v6)
//     //     if (nvs_get_u8(handle, "dvraux", &value8) == ESP_OK)
//     //         m_config.dvrAux = value8;
//     //     if (nvs_get_u8(handle, "dvrstartdelay", &value8) == ESP_OK)
//     //         m_config.dvrStartDelay = value8;
//     //     if (nvs_get_u8(handle, "dvrstopdelay", &value8) == ESP_OK)
//     //         m_config.dvrStopDelay = value8;
//     // }
//     // else
//     // {
//     //     // Need to write the dvr defaults
//     //     m_modified |= MAIN_CHANGED;
//     // }

//     // if (version >= 7) {
//     //     // load button actions
//     //     if (nvs_get_u32(handle, "button1", &value) == ESP_OK)
//     //         m_config.buttonColors[0].raw = value;
//     //     if (nvs_get_u32(handle, "button2", &value) == ESP_OK)
//     //         m_config.buttonColors[1].raw = value;
//     //     // backpackdisable was actually added after 7, but if not found will default to 0 (enabled)
//     //     if (nvs_get_u8(handle, "backpackdisable", &value8) == ESP_OK)
//     //         m_config.backpackDisable = value8;
//     //     if (nvs_get_u8(handle, "backpacktlmen", &value8) == ESP_OK)
//     //         m_config.backpackTlmMode = value8;
//     // }

//     for (unsigned i = 0; i < CONFIG_TX_MODEL_CNT; i++)
//     {
//         char model[10] = "model";
//         itoa(i, model + 5, 10);
//         if (nvs_get_u32(handle, model, &value) == ESP_OK)
//         {
//             if (version >= 7)
//             {
//                 U32_to_Model(value, &m_config.model_config[i]);
//             }
//             else
//             {
//                 // Upgrade v6 to v7 directly writing to nvs instead of calling Commit() over and over
//                 v6_model_config_t v6model;
//                 U32_to_Model(value, &v6model);
//                 model_config_t *const newModel = &m_config.model_config[i];
//                 ModelV6toV7(&v6model, newModel);
//                 nvs_set_u32(handle, model, Model_to_U32(newModel));
//             }
//         }
//     } // for each model

//     if (version != TX_CONFIG_VERSION)
//     {
//         Commit();
//     }
// }

// void TxConfig::Commit()
// {
//     if (!m_modified)
//     {
//         // No changes
//         return;
//     }
//     // #if defined(PLATFORM_ESP32)
//     // Write parts to NVS
//     //     if (m_modified & MODEL_CHANGED)
//     //     {
//     //         uint32_t value = Model_to_U32(m_model);
//     //         char model[10] = "model";
//     //         itoa(m_modelId, model+5, 10);
//     //         nvs_set_u32(handle, model, value);
//     //     }
//     //     if (m_modified & VTX_CHANGED)
//     //     {
//     //         uint32_t value =
//     //             m_config.vtxBand << 24 |
//     //             m_config.vtxChannel << 16 |
//     //             m_config.vtxPower << 8 |
//     //             m_config.vtxPitmode;
//     //         nvs_set_u32(handle, "vtx", value);
//     //     }
//     //     if (m_modified & FAN_CHANGED)
//     //     {
//     //         uint32_t value = m_config.fanMode;
//     //         nvs_set_u32(handle, "fan", value);
//     //     }
//     //     if (m_modified & MOTION_CHANGED)
//     //     {
//     //         uint32_t value = m_config.motionMode;
//     //         nvs_set_u32(handle, "motion", value);
//     //     }
//     //     if (m_modified & MAIN_CHANGED)
//     //     {
//     //         nvs_set_u8(handle, "fanthresh", m_config.powerFanThreshold);

//     //         nvs_set_u8(handle, "backpackdisable", m_config.backpackDisable);
//     //         nvs_set_u8(handle, "backpacktlmen", m_config.backpackTlmMode);
//     //         nvs_set_u8(handle, "dvraux", m_config.dvrAux);
//     //         nvs_set_u8(handle, "dvrstartdelay", m_config.dvrStartDelay);
//     //         nvs_set_u8(handle, "dvrstopdelay", m_config.dvrStopDelay);
//     //     }
//     //     if (m_modified & BUTTON_CHANGED)
//     //     {
//     //         nvs_set_u32(handle, "button1", m_config.buttonColors[0].raw);
//     //         nvs_set_u32(handle, "button2", m_config.buttonColors[1].raw);
//     //     }
//     //     nvs_set_u32(handle, "tx_version", m_config.version);
//     //     nvs_commit(handle);
//     // #else
//     //     // Write the struct to eeprom
//     //     m_eeprom->Put(0, m_config);
//     //     m_eeprom->Commit();
//     // #endif
//     // m_modified = 0;
// }

// // // Setters
// // void TxConfig::SetRate(uint8_t rate)
// // {
// //     if (GetRate() != rate)
// //     {
// //         m_model->rate = rate;
// //         m_modified |= MODEL_CHANGED;
// //     }
// // }

// // void TxConfig::SetTlm(uint8_t tlm)
// // {
// //     if (GetTlm() != tlm)
// //     {
// //         m_model->tlm = tlm;
// //         m_modified |= MODEL_CHANGED;
// //     }
// // }

// // void
// // TxConfig::SetPower(uint8_t power)
// // {
// //     if (GetPower() != power)
// //     {
// //         m_model->power = power;
// //         m_modified |= MODEL_CHANGED;
// //     }
// // }

// // void
// // TxConfig::SetDynamicPower(bool dynamicPower)
// // {
// //     if (GetDynamicPower() != dynamicPower)
// //     {
// //         m_model->dynamicPower = dynamicPower;
// //         m_modified |= MODEL_CHANGED;
// //     }
// // }

// void TxConfig::SetBoostChannel(uint8_t boostChannel)
// {
//     if (GetBoostChannel() != boostChannel)
//     {
//         m_model->boostChannel = boostChannel;
//         m_modified |= MODEL_CHANGED;
//     }
// }

// void TxConfig::SetSwitchMode(uint8_t switchMode)
// {
//     if (GetSwitchMode() != switchMode)
//     {
//         m_model->switchMode = switchMode;
//         m_modified |= MODEL_CHANGED;
//     }
// }

// void
// TxConfig::SetAntennaMode(uint8_t txAntenna)
// {
//     if (GetAntennaMode() != txAntenna)
//     {
//         m_model->txAntenna = txAntenna;
//         m_modified |= MODEL_CHANGED;
//     }
// }

// void
// TxConfig::SetLinkMode(uint8_t linkMode)
// {
//     if (GetLinkMode() != linkMode)
//     {
//         m_model->linkMode = linkMode;

//         if (linkMode == TX_MAVLINK_MODE)
//         {
//             m_model->tlm = TLM_RATIO_1_2;
//             m_model->switchMode = smHybridOr16ch; // Force Hybrid / 16ch/2 switch modes for mavlink
//         }
//         m_modified |= MODEL_CHANGED | MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetModelMatch(bool modelMatch)
// {
//     if (GetModelMatch() != modelMatch)
//     {
//         m_model->modelMatch = modelMatch;
//         m_modified |= MODEL_CHANGED;
//     }
// }

// void
// TxConfig::SetVtxBand(uint8_t vtxBand)
// {
//     if (m_config.vtxBand != vtxBand)
//     {
//         m_config.vtxBand = vtxBand;
//         m_modified |= VTX_CHANGED;
//     }
// }

// void
// TxConfig::SetVtxChannel(uint8_t vtxChannel)
// {
//     if (m_config.vtxChannel != vtxChannel)
//     {
//         m_config.vtxChannel = vtxChannel;
//         m_modified |= VTX_CHANGED;
//     }
// }

// void
// TxConfig::SetVtxPower(uint8_t vtxPower)
// {
//     if (m_config.vtxPower != vtxPower)
//     {
//         m_config.vtxPower = vtxPower;
//         m_modified |= VTX_CHANGED;
//     }
// }

// void
// TxConfig::SetVtxPitmode(uint8_t vtxPitmode)
// {
//     if (m_config.vtxPitmode != vtxPitmode)
//     {
//         m_config.vtxPitmode = vtxPitmode;
//         m_modified |= VTX_CHANGED;
//     }
// }

// void
// TxConfig::SetPowerFanThreshold(uint8_t powerFanThreshold)
// {
//     if (m_config.powerFanThreshold != powerFanThreshold)
//     {
//         m_config.powerFanThreshold = powerFanThreshold;
//         m_modified |= MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetStorageProvider(ELRS_EEPROM *eeprom)
// {
//     if (eeprom)
//     {
//         m_eeprom = eeprom;
//     }
// }

// void
// TxConfig::SetFanMode(uint8_t fanMode)
// {
//     if (m_config.fanMode != fanMode)
//     {
//         m_config.fanMode = fanMode;
//         m_modified |= FAN_CHANGED;
//     }
// }

// void
// TxConfig::SetMotionMode(uint8_t motionMode)
// {
//     if (m_config.motionMode != motionMode)
//     {
//         m_config.motionMode = motionMode;
//         m_modified |= MOTION_CHANGED;
//     }
// }

// void
// TxConfig::SetDvrAux(uint8_t dvrAux)
// {
//     if (GetDvrAux() != dvrAux)
//     {
//         m_config.dvrAux = dvrAux;
//         m_modified |= MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetDvrStartDelay(uint8_t dvrStartDelay)
// {
//     if (GetDvrStartDelay() != dvrStartDelay)
//     {
//         m_config.dvrStartDelay = dvrStartDelay;
//         m_modified |= MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetDvrStopDelay(uint8_t dvrStopDelay)
// {
//     if (GetDvrStopDelay() != dvrStopDelay)
//     {
//         m_config.dvrStopDelay = dvrStopDelay;
//         m_modified |= MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetBackpackDisable(bool backpackDisable)
// {
//     if (m_config.backpackDisable != backpackDisable)
//     {
//         m_config.backpackDisable = backpackDisable;
//         m_modified |= MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetBackpackTlmMode(uint8_t mode)
// {
//     if (m_config.backpackTlmMode != mode)
//     {
//         m_config.backpackTlmMode = mode;
//         m_modified |= MAIN_CHANGED;
//     }
// }

// void
// TxConfig::SetButtonActions(uint8_t button, tx_button_color_t *action)
// {
//     if (m_config.buttonColors[button].raw != action->raw) {
//         m_config.buttonColors[button].raw = action->raw;
//         m_modified |= BUTTON_CHANGED;
//     }
// }

// void
// TxConfig::SetPTRStartChannel(uint8_t ptrStartChannel)
// {
//     if (ptrStartChannel != m_model->ptrStartChannel) {
//         m_model->ptrStartChannel = ptrStartChannel;
//         m_modified |= MODEL_CHANGED;
//     }
// }

// void
// TxConfig::SetPTREnableChannel(uint8_t ptrEnableChannel)
// {
//     if (ptrEnableChannel != m_model->ptrEnableChannel) {
//         m_model->ptrEnableChannel = ptrEnableChannel;
//         m_modified |= MODEL_CHANGED;
//     }
// }

// void
// TxConfig::SetDefaults(bool commit)
// {
//     // Reset everything to 0/false and then just set anything that zero is not appropriate
//     memset(&m_config, 0, sizeof(m_config));

//     m_config.version = TX_CONFIG_VERSION | TX_CONFIG_MAGIC;
//     m_config.powerFanThreshold = PWR_250mW;
//     m_modified = ALL_CHANGED;

//     if (commit)
//     {
//         m_modified = ALL_CHANGED;
//     }

//     // Set defaults for button 1
//     tx_button_color_t default_actions1 = {
//         .val = {
//             .color = 226,   // R:255 G:0 B:182
//             .actions = {
//                 {false, 2, ACTION_BIND},
//                 {true, 0, ACTION_INCREASE_POWER}
//             }
//         }
//     };
//     m_config.buttonColors[0].raw = default_actions1.raw;

//     // Set defaults for button 2
//     tx_button_color_t default_actions2 = {
//         .val = {
//             .color = 3,     // R:0 G:0 B:255
//             .actions = {
//                 {false, 1, ACTION_GOTO_VTX_CHANNEL},
//                 {true, 0, ACTION_SEND_VTX}
//             }
//         }
//     };
//     m_config.buttonColors[1].raw = default_actions2.raw;

//     for (unsigned i=0; i<CONFIG_TX_MODEL_CNT; i++)
//     {
//         SetModelId(i);
//         #if defined(RADIO_SX127X) || defined(RADIO_LR1121)
//             SetRate(enumRatetoIndex(RATE_LORA_200HZ));
//         #elif defined(RADIO_SX128X)
//             SetRate(enumRatetoIndex(RATE_LORA_250HZ));
//         #endif
//         SetPower(POWERMGNT::getDefaultPower());
// #if defined(PLATFORM_ESP32)
//         // ESP32 nvs needs to commit every model
//         if (commit)
//         {
//             m_modified |= MODEL_CHANGED;
//             Commit();
//         }
// #endif
//     }

// #if !defined(PLATFORM_ESP32)
//     // ESP8266 just needs one commit
//     if (commit)
//     {
//         Commit();
//     }
// #endif

//     SetModelId(0);
//     m_modified = 0;
// }

// /**
//  * Sets ModelId used for subsequent per-model config gets
//  * Returns: true if the model has changed
//  **/
// bool
// TxConfig::SetModelId(uint8_t modelId)
// {
//     model_config_t *newModel = &m_config.model_config[modelId];
//     if (newModel != m_model)
//     {
//         m_model = newModel;
//         m_modelId = modelId;
//         return true;
//     }

//     return false;
// }
// #endif
