#pragma once
#include "Pipeline.h"
namespace srx
{
	struct RenderPassSpecification
	{
		Ref<Pipeline> pipeline;
	};
	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecification& spec);
		virtual ~RenderPass() = default;
		inline RenderPassSpecification& GetSpecification() { return _Specification; }
		inline const RenderPassSpecification& GetSpecification() const { return _Specification; }

		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	protected:
		RenderPassSpecification _Specification;
	};
}