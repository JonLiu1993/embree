// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../common/geometry.h"
#include "../common/ray.h"
#include "../common/hit.h"
#include "../common/context.h"

namespace embree
{
  namespace isa
  {
    __forceinline bool runIntersectionFilter1(const Geometry* const geometry, Ray& ray, IntersectContext* context, Hit& hit)
    {
      assert(geometry->intersectionFilterN);
      int mask = -1; int accept = 0;
      geometry->intersectionFilterN(&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,1,&accept);
      if (accept != 0) copyHitToRay(ray,hit);
      return accept != 0;
    }
    
    __forceinline bool runOcclusionFilter1(const Geometry* const geometry, Ray& ray, IntersectContext* context, Hit& hit)
    {
      assert(geometry->occlusionFilterN);
      int mask = -1; int accept = 0;
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,1, (int*)&accept);
      return accept != 0; // ray.geomID == 0;
    }

    __forceinline vbool4 runIntersectionFilter(const vbool4& valid, const Geometry* const geometry, Ray4& ray, IntersectContext* context, Hit4& hit)
    {
      assert(geometry->intersectionFilterN);
      vint4 mask = valid.mask32(); vint4 accept(zero);
      geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4,(int*)&accept);
      accept &= mask;
      const vbool4 final = accept != vint4(zero);
      if (any(final)) copyHitToRay(final,ray,hit);
      return final;
    }
    
    __forceinline vbool4 runOcclusionFilter(const vbool4& valid, const Geometry* const geometry, Ray4& ray, IntersectContext* context, Hit4& hit)
    {
      assert(geometry->occlusionFilterN);
      vint4 mask = valid.mask32(); vint4 accept(zero);
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4,(int*)&accept);
      accept &= mask;
      const vbool4 final = accept != vint4(zero);
      ray.geomID = select(final, vint4(zero), ray.geomID);
      return final; //(mask != vint4(zero)) & (ray.geomID == 0);
    }
    
    __forceinline bool runIntersectionFilter(const Geometry* const geometry, Ray4& ray, const size_t k, IntersectContext* context, Hit4& hit)
    {
      const vbool4 valid(1 << k);
      assert(geometry->intersectionFilterN);
      vint4 mask = valid.mask32(); vint4 accept(zero);
      geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4,(int*)&accept);
      accept &= mask;
      const vbool4 final = accept != vint4(zero);
      if (any(final)) { copyHitToRay(final,ray,hit); return true; }
      return false; //accept[k] != 0; 
    }
    
    __forceinline bool runOcclusionFilter(const Geometry* const geometry, Ray4& ray, const size_t k, IntersectContext* context, Hit4& hit)
    {
      const vbool4 valid(1 << k);
      assert(geometry->occlusionFilterN);
      vint4 mask = valid.mask32(); vint4 accept(zero);
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,4,(int*)&accept);
      accept &= mask;
      const vbool4 final = accept != vint4(zero);
      ray.geomID = select(final, vint4(zero), ray.geomID);
      return any(final); // accept[k] != 0; 
    }
    
#if defined(__AVX__)
    __forceinline vbool8 runIntersectionFilter(const vbool8& valid, const Geometry* const geometry, Ray8& ray, IntersectContext* context, Hit8& hit)
    {
      assert(geometry->intersectionFilterN);
      vint8 mask = valid.mask32(); vint8 accept(zero);
      geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8,(int*)&accept);
      accept &= mask;
      const vbool8 final = accept != vint8(zero);
      if (any(final)) copyHitToRay(final,ray,hit);
      return final; //mask != vint8(0);
    }
    
    __forceinline vbool8 runOcclusionFilter(const vbool8& valid, const Geometry* const geometry, Ray8& ray, IntersectContext* context, Hit8& hit)
    {
      assert(geometry->occlusionFilterN);
      vint8 mask = valid.mask32(); vint8 accept(zero);
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8,(int*)&accept);
      accept &= mask;
      const vbool8 final = accept != vint8(zero);
      ray.geomID = select(final, vint8(zero), ray.geomID);
      return final; //(mask != vint8(zero)) & (ray.geomID == 0);
    }
    
    __forceinline bool runIntersectionFilter(const Geometry* const geometry, Ray8& ray, const size_t k, IntersectContext* context, Hit8& hit)
    {
      const vbool8 valid(1 << k);
      assert(geometry->intersectionFilterN);
      vint8 mask = valid.mask32(); vint8 accept(zero);
      geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8,(int*)&accept);
      accept &= mask;
      const vbool8 final = accept != vint8(zero);
      if (any(final)) { copyHitToRay(final,ray,hit); return true; }
      return false; //accept[k] != 0; 
    }
    
    __forceinline bool runOcclusionFilter(const Geometry* const geometry, Ray8& ray, const size_t k, IntersectContext* context, Hit8& hit)
    {
      const vbool8 valid(1 << k);
      assert(geometry->occlusionFilterN);
      vint8 mask = valid.mask32(); vint8 accept(zero);
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,8,(int*)&accept);
      accept &= mask;
      const vbool8 final = accept != vint8(zero);
      ray.geomID = select(final, vint8(zero), ray.geomID);
      return any(final); //accept[k] != 0; 
    }
    
#endif


#if defined(__AVX512F__)
    __forceinline vbool16 runIntersectionFilter(const vbool16& valid, const Geometry* const geometry, Ray16& ray, IntersectContext* context, Hit16& hit)
    {
      assert(geometry->intersectionFilterN);
      vint16 mask = valid.mask32(); vint16 accept(zero);
      geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16,(int*)&accept);
      accept &= mask;
      const vbool16 final = accept != vint16(zero);
      if (any(final)) copyHitToRay(final,ray,hit);
      return final; // mask != vint16(0);
    }
    
    __forceinline vbool16 runOcclusionFilter(const vbool16& valid, const Geometry* const geometry, Ray16& ray, IntersectContext* context, Hit16& hit)
    {
      assert(geometry->occlusionFilterN);
      vint16 mask = valid.mask32(); vint16 accept(zero);
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16,(int*)&accept);
      accept &= mask;
      const vbool16 final = accept != vint16(zero);
      ray.geomID = select(final, vint16(zero), ray.geomID);
      return final; // (mask != vint16(zero)) & (ray.geomID == 0);
    }
      
    __forceinline bool runIntersectionFilter(const Geometry* const geometry, Ray16& ray, const size_t k, IntersectContext* context, Hit16& hit)
    {
      const vbool16 valid(1 << k);
      assert(geometry->intersectionFilterN);
      vint16 mask = valid.mask32(); vint16 accept(zero);
      geometry->intersectionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16,(int*)&accept);
      accept &= mask;
      const vbool16 final = accept != vint16(zero);
      if (any(final)) { copyHitToRay(final,ray,hit); return true; }
      return false; //accept[k] != 0; 
    }
    
    __forceinline bool runOcclusionFilter(const Geometry* const geometry, Ray16& ray, const size_t k, IntersectContext* context, Hit16& hit)
    {
      const vbool16 valid(1 << k);
      assert(geometry->occlusionFilterN);
      vint16 mask = valid.mask32(); vint16 accept(zero);
      geometry->occlusionFilterN((int*)&mask,geometry->userPtr,context->user,(RTCRayN*)&ray,(RTCHitN*)&hit,16,(int*)&accept);
      accept &= mask;
      const vbool16 final = accept != vint16(zero);
      ray.geomID = select(final, vint16(zero), ray.geomID);
      return any(final); // accept[k] != 0;
    }    
#endif

  }
}
