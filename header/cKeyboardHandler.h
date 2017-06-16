//
//										Benjamin Hernandez, PhD. benjamin.hernandez@bsc.es
//																	   All Rights Reserved
// 														   Barcelona Supercomputing Center
//
//															   See ReadMe.txt for details
//
//=======================================================================================


#ifndef CKEYBOARDHANDLER_H_
#define CKEYBOARDHANDLER_H_

#define KEY_EVENT   4  // according to the RFB protocol

class cKeyboardHandler
{
public:
	enum {
		UP,
		DOWN
	};

	cKeyboardHandler					(	) 				{ key = -1, state = UP; };
	~cKeyboardHandler					(	) 				{						};

	void 	setState					( int state_ 	)	{ state = state_;		};

	void	setKey						( int keyMask	)
	{
		if (keyMask >= 'A' && keyMask<='Z')
		{
			key = keyMask;
		}
		if (keyMask >= '0' && keyMask<='9')
		{
			key = keyMask;
		}
	};

	void	refresh						( bool flag 					)	{ isRefresh = flag;					};

	int		getState					(	)				{ return state; 		};
	int		getKey						(	)				{ return key;			};
	bool	refreshed					(	)				{ return isRefresh;		};



private:
	int		key;
	int		state;
	bool	isRefresh;
};



#endif /* CKEYBOARDHANDLER_H_ */
