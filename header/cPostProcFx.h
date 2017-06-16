// @REV_TAG SRL_10_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjamín Hernández A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//============================================================================================================

//============================================================================================================
//
void blurSSAO( void )
{
	unsigned int ssao_width  = getWidth( "ssao_fbo" );
	unsigned int ssao_height = getHeight( "ssao_fbo" );

	bind_fbo( "ssao_fbo" );
	{
		proj_manager->setOrthoProjection( ssao_width, ssao_height );
		{
//->performing horizontal blur
			setTarget( "ssao_blurh_tex", true, true );
			pushActiveBind( "ssao_tex", 0 );
			{
				renderQuad( "gaussblur_horiz_rect",
							ssao_width,
							ssao_height,
							ssao_width,
							ssao_height				);
			}
			popActiveBind();
//->performing vertical blur
			setTarget( "ssao_tex", true, true );
			pushActiveBind( "ssao_blurh_tex", 0 );
			{
				renderQuad( "gaussblur_vert_rect",
							ssao_width,
							ssao_height,
							ssao_width,
							ssao_height				);
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "ssao_fbo" );
}
//
//============================================================================================================
//
void renderToSSAO( void )
{
	unsigned int ssao_width  = getWidth( "ssao_fbo" );
	unsigned int ssao_height = getHeight( "ssao_fbo" );

	bind_fbo( "ssao_fbo" );
	{
		proj_manager->setOrthoProjection( ssao_width, ssao_height );
		{
			setTarget( "ssao_tex", true, true );
			pushActiveBind( "scene_depth_tex", 0 );
			{
				renderQuad( "ssao_rect",
							ssao_width,
							ssao_height,
							ssao_width,
							ssao_height );
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "ssao_fbo" );
}
//
//============================================================================================================
//
void composeSSAO( void )
{
	unsigned int scene_width  = getWidth( "scene_fbo" );
	unsigned int scene_height = getHeight( "scene_fbo" );

	bind_fbo( "scene_fbo" );
	{
		proj_manager->setOrthoProjection( scene_width, scene_height );
		{
			setTarget( "composed_ssao_tex", true, true );
			pushActiveBind( "scene_tex", 0 );
			{
				pushActiveBind( "ssao_tex", 1 );
				{
					renderQuad( "ssao_compose_rect",
								scene_width,
								scene_height,
								scene_width,
								scene_height		);
				}
				popActiveBind();
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "scene_fbo" );
}
//
//============================================================================================================
//
void blurShadows( void )
{
	unsigned int scene_width  = getWidth( "scene_fbo" );
	unsigned int scene_height = getHeight( "scene_fbo" );

	bind_fbo( "scene_fbo" );
	{
		proj_manager->setOrthoProjection( scene_width, scene_height );
		{
//->performing horizontal blur
			setTarget( "shadow_blurh_tex", true, true );
			pushActiveBind( "shadow_tex", 0 );
			{
				renderQuad( "gaussblur_horiz_rect",
							scene_width,
							scene_height,
							scene_width,
							scene_height			);
			}
			popActiveBind();
//->performing vertical blur
			setTarget( "shadow_tex", true, true );
			pushActiveBind( "shadow_blurh_tex", 0 );
			{
				renderQuad( "gaussblur_vert_rect",
							scene_width,
							scene_height,
							scene_width,
							scene_height			);
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "scene_fbo" );
}
//
//============================================================================================================
//
void composeShadows( bool ssao_enabled )
{
	unsigned int scene_width  = getWidth( "scene_fbo" );
	unsigned int scene_height = getHeight( "scene_fbo" );

	bind_fbo( "scene_fbo" );
	{
		proj_manager->setOrthoProjection( scene_width, scene_height, false );
		{
			setTarget( "composed_shadow_tex", true, true );
			string input_texture;
			if( ssao_enabled )
			{
				input_texture = "composed_ssao_tex";
			}
			else
			{
				input_texture = "scene_tex";
			}
			pushActiveBind( input_texture, 0 );
			{
				pushActiveBind( "shadow_tex", 1 );
				{
					renderQuad( "shadow_compose_rect",
								scene_width,
								scene_height,
								scene_width,
								scene_height		);
				}
				popActiveBind();
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "scene_fbo" );
}
//
//============================================================================================================
//
void downSampleBloom( void )
{
	unsigned int down_width   = getWidth ( "downsample_fbo" );
	unsigned int down_height  = getHeight( "downsample_fbo" );
	unsigned int scene_width  = getWidth ( "scene_fbo"		);
	unsigned int scene_height = getHeight( "scene_fbo"		);

	bind_fbo( "downsample_fbo" );
	{
		setTarget( "downsample_tex", true, true );
		proj_manager->setOrthoProjection( down_width, down_height );
		{
			pushActiveBind( "scene_tex", 0 );
			{
				renderQuad( "downsample_rect",
							down_width,
							down_height,
							scene_width,
							scene_height	);
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "downsample_fbo" );
}
//
//============================================================================================================
//
void renderToBloom( void )
{
	unsigned int down_width  = getWidth ( "downsample_fbo" );
	unsigned int down_height = getHeight( "downsample_fbo" );

	bind_fbo( "downsample_fbo" );
	{
		proj_manager->setOrthoProjection( down_width, down_height );
		{
//->performing horizontal blur
			setTarget( "downsample_bloom_blurh_tex", true, true );
			pushActiveBind( "downsample_tex", 0 );
			{
				renderQuad( "gaussblur_half_horiz_rect",
							down_width,
							down_height,
							down_width,
							down_height				);
			}
			popActiveBind();
//->performing vertical blur
			setTarget( "downsample_final_blur_tex", true, true );
			pushActiveBind( "downsample_bloom_blurh_tex", 0 );
			{
				renderQuad( "gaussblur_half_vert_rect",
							down_width,
							down_height,
							down_width,
							down_height				);
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "downsample_fbo" );
}
//
//============================================================================================================
//
void composeBloom( bool ssao_enabled, bool shadow_enabled )
{
	unsigned int scene_width  = getWidth ( "scene_fbo" );
	unsigned int scene_height = getHeight( "scene_fbo" );

	bind_fbo( "scene_fbo" );
	{
		proj_manager->setOrthoProjection( scene_width, scene_height );
		{
			setTarget( "composed_bloom_tex", true, true );
			string input_texture;
			if( shadow_enabled )
			{
				input_texture = "composed_shadow_tex";
			}
			else if( ssao_enabled )
			{
				input_texture = "composed_ssao_tex";
			}
			else
			{
				input_texture = "scene_tex";
			}
			pushActiveBind( input_texture, 0 );
			{
				pushActiveBind( "downsample_final_blur_tex", 1 );
				{
					renderQuad( "bloom_compose_rect",
								scene_width,
								scene_height,
								scene_width,
								scene_height		);
				}
				popActiveBind();
			}
			popActiveBind();
		}
		proj_manager->restoreProjection();
	}
	unbind_fbo( "scene_fbo" );
}
//
//============================================================================================================
