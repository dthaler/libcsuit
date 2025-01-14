<!--
 Copyright (c) 2020-2023 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

## Example 0: Write and Decrypt Encrypted Payload
{: numbered='no'}

### CBOR Diagnostic Notation of SUIT Manifest
{: numbered='no'}

~~~~
/ SUIT_Envelope_Tagged / 107({
  / authentication-wrapper / 2: << [
    << [
      / digest-algorithm-id: / -16 / SHA256 /,
      / digest-bytes: / h'21F91D0007458DED08ED6C3D6B9A6824FE58100529DF05F5947BDD1FFF5DFBE9'
    ] >>,
    << / COSE_Sign1_Tagged / 18([
      / protected: / << {
        / algorithm-id / 1: -7 / ES256 /
      } >>,
      / unprotected: / {},
      / payload: / null,
      / signature: / h'B1614992A357783A6D35D7D77807D7DA0F67E0CB5D0BDBFD592D9C45C5BDD3B6828E3A7A1101B33D558EB7C5114D057122D69455972D9937A1000E8F10DD5A2E'
    ]) >>
  ] >>,
  / manifest / 3: << {
    / manifest-version / 1: 1,
    / manifest-sequence-number / 2: 1,
    / common / 3: << {
      / components / 2: [
        [h'00'] / to be decrypted firmware /,
      ]
    } >>,
    / install / 17: << [
      / fetch encrypted firmware /
      / directive-override-parameters / 20, {
        / parameter-content / 18: h'2B3765FF457DD98A4BA7130A40462B663C08198146D23F3A32094392CA5040C3121C8E5F4C04D5A3D1D6171BCF362B',
        / parameter-encryption-info / 19: << 96([
          / protected: / << {
            / alg / 1: 1 / AES-GCM-128 /
          } >>,
          / unprotected: / {
            / IV / 5: h'1DE460E8B5B68D7222C0D6F20484D8AB'
          },
          / payload: / null / detached ciphertext /,
          / recipients: / [
            [
              / protected: / << {
              } >>,
              / unprotected: / {
                / alg / 1: -3 / A128KW /,
                / kid / 4: 'kid-1'
              },
              / payload: / h'A86200E4754733E4C00FC08C6A72CC1996E129922EAB504F' / CEK encrypted with KEK /
            ]
          ]
        ]) >>
      },

      / decrypt encrypted firmware /
      / directive-write / 18, 15 / consumes the SUIT_Encryption_Info above /,

      / verify decrypted firmware /
      / directive-override-parameters / 20, {
        / parameter-image-digest / 3: << [
          / digest-algorithm-id: / -16 / SHA-256 /,
          / digest-bytes: / h'efe16b6a486ff25e9fb5fabf515e2bfc3f38b405de377477b23275b53049b46b'
        ] >>,
        / parameter-image-size / 14: 31
      },
      / condition-image-match / 3, 15
    ] >>
  } >>
})
~~~~


### CBOR Binary in Hex
{: numbered='no'}

~~~~
D86BA2025873825824822F582021F91D0007458DED08ED6C3D6B9A6824FE
58100529DF05F5947BDD1FFF5DFBE9584AD28443A10126A0F65840B16149
92A357783A6D35D7D77807D7DA0F67E0CB5D0BDBFD592D9C45C5BDD3B682
8E3A7A1101B33D558EB7C5114D057122D69455972D9937A1000E8F10DD5A
2E0358BBA4010102010346A102818141001158AB8814A212582F2B3765FF
457DD98A4BA7130A40462B663C08198146D23F3A32094392CA5040C3121C
8E5F4C04D5A3D1D6171BCF362B135843D8608443A10101A105501DE460E8
B5B68D7222C0D6F20484D8ABF6818341A0A2012204456B69642D315818A8
6200E4754733E4C00FC08C6A72CC1996E129922EAB504F120F14A2035824
822F5820EFE16B6A486FF25E9FB5FABF515E2BFC3F38B405DE377477B232
75B53049B46B0E181F030F
~~~~
