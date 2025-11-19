#pragma once

namespace srx
{
	class RenderCommandBuffer
	{
	public:
		virtual void Begin() = 0;	//录制命令
		virtual void End() = 0;		//结束录制
		virtual void Submit() = 0;	//提交执行

		static Ref<RenderCommandBuffer> Create();
	private:
	};
}