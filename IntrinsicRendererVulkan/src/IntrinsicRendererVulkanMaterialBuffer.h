// Copyright 2016 Benjamin Glatzel
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

namespace Intrinsic
{
namespace Renderer
{
namespace Vulkan
{
struct MaterialBufferEntry
{
  // Water
  float refractionFactor;

  // Translucency
  float translucencyThicknessFactor;
};

struct MaterialBuffer
{
  static void init();

  _INTR_INLINE static uint32_t allocateMaterialBufferEntry()
  {
    const uint32_t idx = _materialBufferEntries.back();
    _materialBufferEntries.pop_back();

    return idx;
  }

  _INTR_INLINE static void freeMaterialBufferEntry(uint32_t p_Index)
  {
    _materialBufferEntries.push_back(p_Index);
  }

  _INTR_INLINE static void
  updateMaterialBufferEntry(const uint32_t p_Index,
                            const MaterialBufferEntry& p_MaterialBufferEntry)
  {
    {
      void* stagingMemMapped;
      VkResult result =
          vkMapMemory(RenderSystem::_vkDevice, _materialStagingBufferMemory, 0u,
                      sizeof(MaterialBufferEntry), 0u, &stagingMemMapped);
      _INTR_VK_CHECK_RESULT(result);

      memcpy(stagingMemMapped, &p_MaterialBufferEntry,
             sizeof(MaterialBufferEntry));

      vkUnmapMemory(RenderSystem::_vkDevice, _materialStagingBufferMemory);
    }

    VkCommandBuffer copyCmd = RenderSystem::beginTemporaryCommandBuffer();

    VkBufferCopy bufferCopy = {};
    {
      bufferCopy.dstOffset = p_Index * sizeof(MaterialBufferEntry);
      bufferCopy.srcOffset = 0u;
      bufferCopy.size = sizeof(MaterialBufferEntry);
    }

    vkCmdCopyBuffer(copyCmd, _materialStagingBuffer,
                    Resources::BufferManager::_vkBuffer(_materialBuffer), 1u,
                    &bufferCopy);

    RenderSystem::flushTemporaryCommandBuffer();
  }

  static Resources::BufferRef _materialBuffer;

private:
  static _INTR_ARRAY(uint32_t) _materialBufferEntries;
  static VkBuffer _materialStagingBuffer;
  static VkDeviceMemory _materialStagingBufferMemory;
};
}
}
}
