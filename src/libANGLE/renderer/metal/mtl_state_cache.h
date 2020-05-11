//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// mtl_state_cache.h:
//    Defines the class interface for StateCache, RenderPipelineCache and various
//    C struct versions of Metal sampler, depth stencil, render pass, render pipeline descriptors.
//

#ifndef LIBANGLE_RENDERER_METAL_MTL_STATE_CACHE_H_
#define LIBANGLE_RENDERER_METAL_MTL_STATE_CACHE_H_

#import <Metal/Metal.h>

#include <unordered_map>

#include "libANGLE/State.h"
#include "libANGLE/angletypes.h"
#include "libANGLE/renderer/metal/mtl_common.h"
#include "libANGLE/renderer/metal/mtl_resources.h"

static inline bool operator==(const MTLClearColor &lhs, const MTLClearColor &rhs);

namespace rx
{
class ContextMtl;

namespace mtl
{
struct alignas(1) StencilDesc
{
    bool operator==(const StencilDesc &rhs) const;

    // Set default values
    void reset();

    // Use uint8_t instead of MTLStencilOperation to compact space
    uint8_t stencilFailureOperation : 3;
    uint8_t depthFailureOperation : 3;
    uint8_t depthStencilPassOperation : 3;

    // Use uint8_t instead of MTLCompareFunction to compact space
    uint8_t stencilCompareFunction : 3;

    uint8_t readMask : 8;
    uint8_t writeMask : 8;
};

struct alignas(4) DepthStencilDesc
{
    DepthStencilDesc();
    DepthStencilDesc(const DepthStencilDesc &src);
    DepthStencilDesc(DepthStencilDesc &&src);

    DepthStencilDesc &operator=(const DepthStencilDesc &src);

    bool operator==(const DepthStencilDesc &rhs) const;

    // Set default values.
    // Default is depth/stencil test disabled. Depth/stencil write enabled.
    void reset();

    size_t hash() const;

    void updateDepthTestEnabled(const gl::DepthStencilState &dsState);
    void updateDepthWriteEnabled(const gl::DepthStencilState &dsState);
    void updateDepthCompareFunc(const gl::DepthStencilState &dsState);
    void updateStencilTestEnabled(const gl::DepthStencilState &dsState);
    void updateStencilFrontOps(const gl::DepthStencilState &dsState);
    void updateStencilBackOps(const gl::DepthStencilState &dsState);
    void updateStencilFrontFuncs(const gl::DepthStencilState &dsState);
    void updateStencilBackFuncs(const gl::DepthStencilState &dsState);
    void updateStencilFrontWriteMask(const gl::DepthStencilState &dsState);
    void updateStencilBackWriteMask(const gl::DepthStencilState &dsState);

    StencilDesc backFaceStencil;
    StencilDesc frontFaceStencil;

    // Use uint8_t instead of MTLCompareFunction to compact space
    uint8_t depthCompareFunction : 3;
    bool depthWriteEnabled : 1;
};

struct alignas(4) SamplerDesc
{
    SamplerDesc();
    SamplerDesc(const SamplerDesc &src);
    SamplerDesc(SamplerDesc &&src);

    explicit SamplerDesc(const gl::SamplerState &glState);

    SamplerDesc &operator=(const SamplerDesc &src);

    // Set default values. All filters are nearest, and addresModes are clamp to edge.
    void reset();

    bool operator==(const SamplerDesc &rhs) const;

    size_t hash() const;

    // Use uint8_t instead of MTLSamplerAddressMode to compact space
    uint8_t rAddressMode : 3;
    uint8_t sAddressMode : 3;
    uint8_t tAddressMode : 3;

    // Use uint8_t instead of MTLSamplerMinMagFilter to compact space
    uint8_t minFilter : 1;
    uint8_t magFilter : 1;
    uint8_t mipFilter : 2;

    uint8_t maxAnisotropy : 5;
};

struct VertexAttributeDesc
{
    inline bool operator==(const VertexAttributeDesc &rhs) const
    {
        return format == rhs.format && offset == rhs.offset && bufferIndex == rhs.bufferIndex;
    }
    inline bool operator!=(const VertexAttributeDesc &rhs) const { return !(*this == rhs); }

    // Use uint8_t instead of MTLVertexFormat to compact space
    uint8_t format : 6;
    // Offset is only used for default attributes buffer. So 8 bits are enough.
    uint8_t offset : 8;
    uint8_t bufferIndex : 5;
};

struct VertexBufferLayoutDesc
{
    inline bool operator==(const VertexBufferLayoutDesc &rhs) const
    {
        return stepFunction == rhs.stepFunction && stepRate == rhs.stepRate && stride == rhs.stride;
    }
    inline bool operator!=(const VertexBufferLayoutDesc &rhs) const { return !(*this == rhs); }

    uint32_t stepRate;
    uint32_t stride;

    // Use uint8_t instead of MTLVertexStepFunction to compact space
    uint8_t stepFunction;
};

struct VertexDesc
{
    VertexAttributeDesc attributes[kMaxVertexAttribs];
    VertexBufferLayoutDesc layouts[kMaxVertexAttribs];

    uint8_t numAttribs;
    uint8_t numBufferLayouts;
};

struct BlendDesc
{
    bool operator==(const BlendDesc &rhs) const;
    BlendDesc &operator=(const BlendDesc &src) = default;

    // Set default values
    void reset();
    void reset(MTLColorWriteMask writeMask);

    void updateWriteMask(const gl::BlendState &blendState);
    void updateBlendFactors(const gl::BlendState &blendState);
    void updateBlendOps(const gl::BlendState &blendState);
    void updateBlendEnabled(const gl::BlendState &blendState);

    // Use uint8_t instead of MTLColorWriteMask to compact space
    uint8_t writeMask : 4;

    // Use uint8_t instead of MTLBlendOperation to compact space
    uint8_t alphaBlendOperation : 3;
    uint8_t rgbBlendOperation : 3;

    // Use uint8_t instead of MTLBlendFactor to compact space
    // NOTE(hqle): enum MTLBlendFactorSource1Color and above are unused.
    uint8_t destinationAlphaBlendFactor : 4;
    uint8_t destinationRGBBlendFactor : 4;
    uint8_t sourceAlphaBlendFactor : 4;
    uint8_t sourceRGBBlendFactor : 4;

    bool blendingEnabled : 1;
};

struct alignas(2) RenderPipelineColorAttachmentDesc : public BlendDesc
{
    bool operator==(const RenderPipelineColorAttachmentDesc &rhs) const;
    inline bool operator!=(const RenderPipelineColorAttachmentDesc &rhs) const
    {
        return !(*this == rhs);
    }

    // Set default values
    void reset();
    void reset(MTLPixelFormat format);
    void reset(MTLPixelFormat format, MTLColorWriteMask writeMask);
    void reset(MTLPixelFormat format, const BlendDesc &blendState);

    void update(const BlendDesc &blendState);

    // Use uint16_t instead of MTLPixelFormat to compact space
    uint16_t pixelFormat : 16;
};

struct RenderPipelineOutputDesc
{
    bool operator==(const RenderPipelineOutputDesc &rhs) const;

    RenderPipelineColorAttachmentDesc colorAttachments[kMaxRenderTargets];

    // Use uint16_t instead of MTLPixelFormat to compact space
    uint16_t depthAttachmentPixelFormat : 16;
    uint16_t stencilAttachmentPixelFormat : 16;

    static_assert(kMaxRenderTargets <= 4, "kMaxRenderTargets must be <= 4");
    uint8_t numColorAttachments : 3;
};

// Some SDK levels don't declare MTLPrimitiveTopologyClass. Needs to do compile time check here:
#if !(TARGET_OS_OSX || TARGET_OS_MACCATALYST) && ANGLE_IOS_DEPLOY_TARGET < __IPHONE_12_0
#    define ANGLE_MTL_PRIMITIVE_TOPOLOGY_CLASS_AVAILABLE 0
using PrimitiveTopologyClass                                     = uint32_t;
constexpr PrimitiveTopologyClass kPrimitiveTopologyClassTriangle = 0;
constexpr PrimitiveTopologyClass kPrimitiveTopologyClassPoint    = 0;
#else
#    define ANGLE_MTL_PRIMITIVE_TOPOLOGY_CLASS_AVAILABLE 1
using PrimitiveTopologyClass = MTLPrimitiveTopologyClass;
constexpr PrimitiveTopologyClass kPrimitiveTopologyClassTriangle =
    MTLPrimitiveTopologyClassTriangle;
constexpr PrimitiveTopologyClass kPrimitiveTopologyClassPoint = MTLPrimitiveTopologyClassPoint;
#endif

struct alignas(4) RenderPipelineDesc
{
    RenderPipelineDesc();
    RenderPipelineDesc(const RenderPipelineDesc &src);
    RenderPipelineDesc(RenderPipelineDesc &&src);

    RenderPipelineDesc &operator=(const RenderPipelineDesc &src);

    bool operator==(const RenderPipelineDesc &rhs) const;

    size_t hash() const;

    VertexDesc vertexDescriptor;

    RenderPipelineOutputDesc outputDescriptor;

    // Use uint8_t instead of PrimitiveTopologyClass to compact space.
    uint8_t inputPrimitiveTopology : 2;

    bool rasterizationEnabled;
};

struct RenderPassAttachmentDesc
{
    RenderPassAttachmentDesc();
    // Set default values
    void reset();

    bool equalIgnoreLoadStoreOptions(const RenderPassAttachmentDesc &other) const;
    bool operator==(const RenderPassAttachmentDesc &other) const;

    TextureRef texture;
    uint32_t level;
    uint32_t slice;
    MTLLoadAction loadAction;
    MTLStoreAction storeAction;
    MTLStoreActionOptions storeActionOptions;
};

struct RenderPassColorAttachmentDesc : public RenderPassAttachmentDesc
{
    inline bool operator==(const RenderPassColorAttachmentDesc &other) const
    {
        return RenderPassAttachmentDesc::operator==(other) && clearColor == other.clearColor;
    }
    inline bool operator!=(const RenderPassColorAttachmentDesc &other) const
    {
        return !(*this == other);
    }
    MTLClearColor clearColor = {0, 0, 0, 0};
};

struct RenderPassDepthAttachmentDesc : public RenderPassAttachmentDesc
{
    inline bool operator==(const RenderPassDepthAttachmentDesc &other) const
    {
        return RenderPassAttachmentDesc::operator==(other) && clearDepth == other.clearDepth;
    }
    inline bool operator!=(const RenderPassDepthAttachmentDesc &other) const
    {
        return !(*this == other);
    }

    double clearDepth = 0;
};

struct RenderPassStencilAttachmentDesc : public RenderPassAttachmentDesc
{
    inline bool operator==(const RenderPassStencilAttachmentDesc &other) const
    {
        return RenderPassAttachmentDesc::operator==(other) && clearStencil == other.clearStencil;
    }
    inline bool operator!=(const RenderPassStencilAttachmentDesc &other) const
    {
        return !(*this == other);
    }
    uint32_t clearStencil = 0;
};

struct RenderPassDesc
{
    RenderPassColorAttachmentDesc colorAttachments[kMaxRenderTargets];
    RenderPassDepthAttachmentDesc depthAttachment;
    RenderPassStencilAttachmentDesc stencilAttachment;

    void convertToMetalDesc(MTLRenderPassDescriptor *objCDesc) const;

    // This will populate the RenderPipelineOutputDesc with default blend state and
    // MTLColorWriteMaskAll
    void populateRenderPipelineOutputDesc(RenderPipelineOutputDesc *outDesc) const;
    // This will populate the RenderPipelineOutputDesc with default blend state and the specified
    // MTLColorWriteMask
    void populateRenderPipelineOutputDesc(MTLColorWriteMask colorWriteMask,
                                          RenderPipelineOutputDesc *outDesc) const;
    // This will populate the RenderPipelineOutputDesc with the specified blend state
    void populateRenderPipelineOutputDesc(const BlendDesc &blendState,
                                          RenderPipelineOutputDesc *outDesc) const;

    bool equalIgnoreLoadStoreOptions(const RenderPassDesc &other) const;
    bool operator==(const RenderPassDesc &other) const;
    inline bool operator!=(const RenderPassDesc &other) const { return !(*this == other); }

    uint32_t numColorAttachments = 0;
};

}  // namespace mtl
}  // namespace rx

namespace std
{

template <>
struct hash<rx::mtl::DepthStencilDesc>
{
    size_t operator()(const rx::mtl::DepthStencilDesc &key) const { return key.hash(); }
};

template <>
struct hash<rx::mtl::SamplerDesc>
{
    size_t operator()(const rx::mtl::SamplerDesc &key) const { return key.hash(); }
};

template <>
struct hash<rx::mtl::RenderPipelineDesc>
{
    size_t operator()(const rx::mtl::RenderPipelineDesc &key) const { return key.hash(); }
};

}  // namespace std

namespace rx
{
namespace mtl
{
// render pipeline state cache per shader program
class RenderPipelineCache final : angle::NonCopyable
{
  public:
    RenderPipelineCache();
    ~RenderPipelineCache();

    void setVertexShader(Context *context, id<MTLFunction> shader);
    void setFragmentShader(Context *context, id<MTLFunction> shader);

    id<MTLFunction> getVertexShader() { return mVertexShader.get(); }
    id<MTLFunction> getFragmentShader() { return mFragmentShader.get(); }

    AutoObjCPtr<id<MTLRenderPipelineState>> getRenderPipelineState(ContextMtl *context,
                                                                   const RenderPipelineDesc &desc);

    void clear();

  protected:
    AutoObjCPtr<id<MTLFunction>> mVertexShader   = nil;
    AutoObjCPtr<id<MTLFunction>> mFragmentShader = nil;

  private:
    void clearPipelineStates();
    void recreatePipelineStates(Context *context);
    AutoObjCPtr<id<MTLRenderPipelineState>> insertRenderPipelineState(
        Context *context,
        const RenderPipelineDesc &desc,
        bool insertDefaultAttribLayout);
    AutoObjCPtr<id<MTLRenderPipelineState>> createRenderPipelineState(
        Context *context,
        const RenderPipelineDesc &desc,
        bool insertDefaultAttribLayout);

    bool hasDefaultAttribs(const RenderPipelineDesc &desc) const;

    // One table with default attrib and one table without.
    std::unordered_map<RenderPipelineDesc, AutoObjCPtr<id<MTLRenderPipelineState>>>
        mRenderPipelineStates[2];
};

class StateCache final : angle::NonCopyable
{
  public:
    StateCache();
    ~StateCache();

    // Null depth stencil state has depth/stecil read & write disabled.
    inline AutoObjCPtr<id<MTLDepthStencilState>> getNullDepthStencilState(Context *context)
    {
        return getNullDepthStencilState(context->getMetalDevice());
    }
    AutoObjCPtr<id<MTLDepthStencilState>> getNullDepthStencilState(id<MTLDevice> device);
    AutoObjCPtr<id<MTLDepthStencilState>> getDepthStencilState(id<MTLDevice> device,
                                                               const DepthStencilDesc &desc);
    AutoObjCPtr<id<MTLSamplerState>> getSamplerState(id<MTLDevice> device, const SamplerDesc &desc);
    // Null sampler state uses default SamplerDesc
    AutoObjCPtr<id<MTLSamplerState>> getNullSamplerState(Context *context);
    AutoObjCPtr<id<MTLSamplerState>> getNullSamplerState(id<MTLDevice> device);
    void clear();

  private:
    AutoObjCPtr<id<MTLDepthStencilState>> mNullDepthStencilState = nil;
    std::unordered_map<DepthStencilDesc, AutoObjCPtr<id<MTLDepthStencilState>>> mDepthStencilStates;
    std::unordered_map<SamplerDesc, AutoObjCPtr<id<MTLSamplerState>>> mSamplerStates;
};

}  // namespace mtl
}  // namespace rx

static inline bool operator==(const rx::mtl::VertexDesc &lhs, const rx::mtl::VertexDesc &rhs)
{
    if (lhs.numAttribs != rhs.numAttribs || lhs.numBufferLayouts != rhs.numBufferLayouts)
    {
        return false;
    }
    for (uint8_t i = 0; i < lhs.numAttribs; ++i)
    {
        if (lhs.attributes[i] != rhs.attributes[i])
        {
            return false;
        }
    }
    for (uint8_t i = 0; i < lhs.numBufferLayouts; ++i)
    {
        if (lhs.layouts[i] != rhs.layouts[i])
        {
            return false;
        }
    }
    return true;
}

static inline bool operator==(const MTLClearColor &lhs, const MTLClearColor &rhs)
{
    return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue &&
           lhs.alpha == rhs.alpha;
}

#endif /* LIBANGLE_RENDERER_METAL_MTL_STATE_CACHE_H_ */
