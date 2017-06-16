//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#ifndef CMOUSEEVENTHANDLER_H_
#define CMOUSEEVENTHANDLER_H_

#define MOUSE_EVENT 5 // according to the RFB protocol


class cMouseHandler
{
public:
	enum
	{
		LEFT_BUTTON,
		MIDDLE_BUTTON,
		RIGHT_BUTTON,
		DOWN,
		UP,
	};

	cMouseHandler						(	) { x = 0, y = 0, button = -1, state = UP;	isRefresh = false;};
	~cMouseHandler 						(	) {	};

	void 	setCoords						( int x_, int y_				)	{ x = x_, y = y_; 					};

	void	setButton						( int buttonMask				)
	{
		switch (buttonMask)
		{
			case 0:
				state = UP;
				break;
			case 1:
				button 	= LEFT_BUTTON;
				state 	= DOWN;
				break;
			case 2:
				button	= MIDDLE_BUTTON;
				state	= DOWN;
				break;
			case 4:
				button	= RIGHT_BUTTON;
				state	= DOWN;
				break;
		}

	};

	void	refresh							( bool flag 					)	{ isRefresh = flag;					};

	int 	getX							(	) 		{ return x; 			};
	int 	getY							(	)		{ return y; 			};
	int		getState						(	)		{ return state;			};
	int		getButton						(	)		{ return button;		};

	bool	refreshed						(	)		{ return isRefresh;	};


private:

	int 	x, y;
	int 	button;
	int		state;
	bool	isRefresh;

};




#endif /* CMOUSEEVENTHANDLER_H_ */
