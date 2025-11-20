#pragma once
#include "Shader.h"
#include "FrameBuffer.h"
namespace srx
{
	struct PipelineSpecification
	{
		Ref<Shader> shader;
		Ref<FrameBuffer> frame_buffer;
	};

	class Pipeline
	{
	public:
		Pipeline(const PipelineSpecification& spec);
		virtual ~Pipeline() = default;
		inline PipelineSpecification& GetSpecification() { return _Specification; }
		inline PipelineSpecification const& GetSpecification() const { return _Specification; }
		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	private:
		PipelineSpecification _Specification;
	};
}