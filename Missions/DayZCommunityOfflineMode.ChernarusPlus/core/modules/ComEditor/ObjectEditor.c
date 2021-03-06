
class ObjectEditor extends Module
{
	protected bool m_ObjectEditorActive = false;
	protected bool m_IsDragging;
	Object m_SelectedObject;
	Object building;

	protected ref ObjectMenu m_ObjectMenu;

	// protected ref Scene active_Scene;
	ref array< ref Object> m_Objects = new array< ref Object>;

	string BASE_COM_DIR = "$saves:CommunityOfflineMode";
	string BASE_SCENE_DIR = BASE_COM_DIR + "\\Scenes";

	ref SceneManager sceneManager;
	SceneData currentSceneData;

	void ObjectEditor()
	{
		MakeDirectory(BASE_COM_DIR);
		MakeDirectory(BASE_SCENE_DIR);

		sceneManager = new SceneManager();
	}

	void ~ObjectEditor()
	{

	}

	void addObject( Object trackedObject )
	{
		m_Objects.Insert( trackedObject );
	}

	override void Init()
	{
		super.Init();

		LoadScene();
	}

	/**

		Works very similar to pluginkeybinding system with extended features such as multiple keybinds (mouse + keyboard) per function
		Please refrain from assigning same mouse keys to different functions even with more keybinds.
		ie: shift+click function vs click function will both call when pressing the former.

		WORK AROUND COULD BE TO MAKE PRIORITY TO FUNCTIONS THAT REQUIRE MORE KEYS/MOUSE BINDS
		IE: SHIFT+END TOGGLEOBJECTEDITOR VS CTRL+SHIFT+END SOMEOTHERFUNCTION
		THE FUNCTION WITH MORE BINDS SHOULD BE REGISTERED FIRST.

		TODO: MAKE SYSTEM LIKE THAT ^
		- dannydog

	**/
	override void RegisterKeyMouseBindings()
	{
//		KeyMouseBinding toggleEditor  = new KeyMouseBinding( GetModuleType(), "ToggleEditor" , "[Shift]+[End]" , "Toggle object editor."            );
		KeyMouseBinding objectSelect  = new KeyMouseBinding( GetModuleType(), "ClickObject"  , "(LMB)+(Click)" , "Selects object on cursor.", true  );
		KeyMouseBinding objectDrag    = new KeyMouseBinding( GetModuleType(), "DragObject"   , "(LMB)+(Drag)"  , "Drag objects on cursor.",   true  );
		KeyMouseBinding objectDelete  = new KeyMouseBinding( GetModuleType(), "DeleteObject" , "[Delete]"	   , "Deletes selected object.",  true  );
		KeyMouseBinding objectGround  = new KeyMouseBinding( GetModuleType(), "GroundObject" , "(Middle Mouse)", "Snaps objects to ground.",  true  );
		KeyMouseBinding sceneSave     = new KeyMouseBinding( GetModuleType(), "ExportScene"  , "CTRL+S"	       , "Saves current scene of objects", true);
		KeyMouseBinding tabFix        = new KeyMouseBinding( GetModuleType(), "TabFix"       , "ALT"	       , "Fixes issue with tabbing out of the game", true );
//		KeyMouseBinding objectScroll  = new KeyMouseBinding( GetModuleType(), "ScrollObject" , "[Shift][Ctrl][Alt]+(Wheel)" , "Raise or lower objects with mouse wheel as well as rotate.", true );

//		toggleEditor.AddKeyBind( KeyCode.KC_LSHIFT, KeyMouseBinding.KB_EVENT_HOLD    );
//		toggleEditor.AddKeyBind( KeyCode.KC_END   , KeyMouseBinding.KB_EVENT_RELEASE ); // Press END. Using Release prevents key HOLD spam from onKeyPress (could use ClearKey in onKeyPress however)
		objectDelete.AddKeyBind( KeyCode.KC_DELETE, KeyMouseBinding.KB_EVENT_RELEASE ); // Pretty much making KB_EVENT_PRESS useless since you can just use KB_EVENT_HOLD instead.
		sceneSave.AddKeyBind( KeyCode.KC_LCONTROL,  KeyMouseBinding.KB_EVENT_HOLD );
		sceneSave.AddKeyBind( KeyCode.KC_S, 	 KeyMouseBinding.KB_EVENT_RELEASE );
		tabFix.AddKeyBind( KeyCode.KC_LMENU, 	 KeyMouseBinding.KB_EVENT_PRESS );

		objectSelect.AddMouseBind( MouseState.LEFT		, KeyMouseBinding.MB_EVENT_CLICK ); // Left Click
		objectDrag.  AddMouseBind( MouseState.LEFT 		, KeyMouseBinding.MB_EVENT_DRAG  );
		objectGround.AddMouseBind( MouseState.MIDDLE	, KeyMouseBinding.MB_EVENT_CLICK );
//		objectScroll.AddMouseBind( MouseState.WHEEL		, 0 							 );

//		RegisterKeyMouseBinding( toggleEditor );
		RegisterKeyMouseBinding( objectSelect );
		RegisterKeyMouseBinding( objectDrag   );
		RegisterKeyMouseBinding( objectDelete );
		RegisterKeyMouseBinding( objectGround );
		RegisterKeyMouseBinding( sceneSave );
		RegisterKeyMouseBinding( tabFix );
//		RegisterKeyMouseBinding( objectScroll );

	}

    void TabFix()
    {
        m_SelectedObject = NULL;
    }

	void ExportScene()
	{
		string toCopy;
        toCopy += "//Spawn helper function\n";
        toCopy += "void SpawnObject( string type, vector position, vector orientation )\n";
        toCopy += "{\n";
        toCopy += "    auto obj = GetGame().CreateObject( type, position );\n";
        toCopy += "    obj.SetPosition( position );\n";
        toCopy += "    obj.SetOrientation( orientation );\n";
        toCopy += "    //Force collision update\n";
        toCopy += "    vector roll = obj.GetOrientation();\n";
        toCopy += "    roll [ 2 ] = roll [ 2 ] - 1;\n";
        toCopy += "    obj.SetOrientation( roll );\n";
        toCopy += "    roll [ 2 ] = roll [ 2 ] + 1;\n";
        toCopy += "    obj.SetOrientation( roll );\n";
        toCopy += "}\n";
        toCopy += "\n";
        toCopy += "//Your custom spawned objects\n";

		foreach( Object m_object : m_Objects )
		{
			toCopy = toCopy + "SpawnObject(\"" + m_object.GetType() + "\", \"" + VectorToString( m_object.GetPosition() ) + "\", \"" + VectorToString( m_object.GetOrientation() ) + "\");\n";
		}

		GetGame().CopyToClipboard( toCopy );

		Message( "Copied to clipboard" );
	}

	void SaveScene()
	{

		// loot spots
		// pumpkins = civilian
		// apple = medical
		// orange = industral
		// plum = military

/*
		SceneInfo sceneData = new SceneInfo("Test");
		// vector position = GetPlayer().WorldToModel(); // origin point

		foreach( Object m_object : m_Objects )
		{
			vector pos = GetPlayer().WorldToModel( m_object.GetPosition() );

			if ( m_object.GetType() != "Pumpkin" )
			{
				sceneData.AddObject( m_object, pos ); // add objects
			}
			// then add loot spots
		}

		foreach( Object m_objectt : m_Objects )
		{
			vector poss = GetPlayer().WorldToModel( m_object.GetPosition() );

			if ( m_object.GetType() == "Pumpkin" )
			{
				sceneData.AddObject( m_objectt, poss ); // add objects
			}
			// then add loot spots
		}

		JsonFileLoader< SceneInfo >.JsonSaveFile( SCENE_DATA + "\\" + sceneData.GetName() + ".json", sceneData );

		*/

		/*
		string toCopy = "";

		foreach( Object m_object : m_Objects )
		{
			if ( m_object.GetType() == "Pot" ) //save loot positions
			{
				vector modelPos = m_SelectedObject.WorldToModel( m_object.GetPosition() );

				toCopy = toCopy + "{" + modelPos[0] + "," + modelPos[1] + "," + modelPos[2] + "},";
			}
		}
		toCopy = m_SelectedObject.GetType() + "[] = {" + toCopy + "};";

		GetGame().CopyToClipboard( toCopy );
		m_Objects.Clear();
		*/


		ref SceneSaveST scene = new SceneSaveST();
		scene.name = "latest";

		foreach( Object m_object : m_Objects )
		{
			ref Param objectParam = new Param3<string, vector, vector>( m_object.GetType(), m_object.GetPosition(), m_object.GetOrientation() );
			scene.m_SceneObjects.Insert( objectParam );

		}

		Message("Saved objects to latest.json");
		JsonFileLoader< SceneSaveST >.JsonSaveFile( BASE_SCENE_DIR + "\\" + "latest.json", scene );

	}

	void LoadScene()
	{

		ref SceneSaveST scene = new SceneSaveST();

		JsonFileLoader<SceneSaveST>.JsonLoadFile( BASE_SCENE_DIR + "\\" + "latest.json", scene );

		foreach( auto param : scene.m_SceneObjects )
		{

			Object object = GetGame().CreateObject( param.param1, param.param2, false, false );
			object.SetOrientation( param.param3 );

			m_Objects.Insert( object );
		}

	}

	void EditorState( bool state )
	{
		if ( m_ObjectEditorActive == state )
		{
			return;
		}

		m_ObjectEditorActive = state;

		if ( m_ObjectEditorActive )
		{
			GetPlayer().MessageStatus("Object Editor Enabled");
		}
		else
		{
			GetPlayer().MessageStatus("Object Editor Disabled");
		}
	}

	void ToggleEditor()
	{
		m_ObjectEditorActive = !m_ObjectEditorActive;

		if ( m_ObjectEditorActive )
		{
			GetPlayer().MessageStatus("Object Editor Enabled");
		}
		else
		{
			GetPlayer().MessageStatus("Object Editor Disabled");
		}
	}

	bool IsEditing()
	{
		return m_ObjectEditorActive;
	}

	void SelectObject( Object object )
	{
		if ( ( ( m_SelectedObject != NULL ) && ( m_SelectedObject == object ) ) || object.IsInherited( PlayerBase ) )
		{
			return;
		}

		m_SelectedObject = object;
	}

	void DeselectObject()
	{
		m_SelectedObject = NULL;
	}

	void DragObject()
	{
		if ( !m_ObjectEditorActive )
		{
			return;
		}

		if ( m_SelectedObject )
		{
			vector dir = GetGame().GetPointerDirection();

			vector from = GetGame().GetCurrentCameraPosition();

			vector to = from + ( dir * 10000 );

			vector contact_pos;
			vector contact_dir;
			vector bbox;

			int contact_component;

			m_SelectedObject.GetCollisionBox( bbox );

			if ( DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, NULL, m_SelectedObject, GetPlayer(), false, false ) )
			{
				//vector oOrgPos = m_SelectedObject.GetPosition();
				//float fSurfaceHight = GetGame().SurfaceY( oOrgPos [ 0 ], oOrgPos [ 2 ] );

				//float nHightOffsetToGround = oOrgPos [ 1 ] - fSurfaceHight; // todo: store offset of existing objects for later use. for snap to ground perhaps?

				//contact_pos [ 1 ] = contact_pos [ 1 ] + nHightOffsetToGround;

				m_SelectedObject.SetPosition( contact_pos );
				m_SelectedObject.PlaceOnSurface();

				ForceTargetCollisionUpdate( m_SelectedObject );

				ObjectInfoMenu.infoPosX.SetText( m_SelectedObject.GetPosition()[0].ToString() );
				ObjectInfoMenu.infoPosY.SetText( m_SelectedObject.GetPosition()[1].ToString() );
				ObjectInfoMenu.infoPosZ.SetText( m_SelectedObject.GetPosition()[2].ToString() );
			}
		}
	}

//	void ScrollObject( int state )
//	{
//		if ( !m_ObjectEditorActive )
//		{
//			return;
//		}
//		/*
//
//		if ( m_SelectedObject )
//		{
//			vector pos = m_SelectedObject.GetPosition();
//			vector ori = m_SelectedObject.GetOrientation();
//
//			bool up = state < 0;
//			int value = 1;
//			if ( up ) value = -1;
//
//			if ( SHIFT() )
//			{
//				if ( ori[0] > 0 ) // seemless pitch change
//				{
//					value = -value;
//				}
//				ori[1] = ori[1] + value;
//
//				m_SelectedObject.SetOrientation( ori );
//			}
//			else if ( CTRL() )
//			{
//				ori [ 0 ] = ori [ 0 ] + value;
//
//				m_SelectedObject.SetOrientation( ori );
//			}
//			else if ( ALT() )
//			{
//				ori[ 2 ] = ori[ 2 ] + value;
//
//				m_SelectedObject.SetOrientation( ori );
//			}
//			else
//			{
//				pos [ 1 ] = pos [ 1 ] + value*0.05;
//
//				m_SelectedObject.SetPosition( pos );
//			}
//		}
//		*/
//	}

	void ClickObject()
	{
		if ( !m_ObjectEditorActive )
		{
			return;
		}
		Widget widgetCursor = GetGame().GetUIManager().GetWidgetUnderCursor();

		if ( widgetCursor && widgetCursor.GetName() != "EditorMenu" )
		{
			return;
		}

		vector dir = GetGame().GetPointerDirection();
		vector from = GetGame().GetCurrentCameraPosition();
		vector to = from + ( dir * 100 );

		set< Object > objects = GetObjectsAt(from, to, GetGame().GetPlayer(), 0.5 );
		bool selected = false;
		//Print ("Building Type = " + building.GetType());

		for ( int newObject = 0; ( ( newObject < objects.Count() ) && !selected ); ++newObject )
		{
			Object obj = objects.Get( newObject );

			if( obj.IsBuilding() )
			{
				building = obj;

				SelectObject( obj );
				selected = true;

				ObjectInfoMenu.infoPosX.SetText( m_SelectedObject.GetPosition()[0].ToString() );
				ObjectInfoMenu.infoPosY.SetText( m_SelectedObject.GetPosition()[1].ToString() );
				ObjectInfoMenu.infoPosZ.SetText( m_SelectedObject.GetPosition()[2].ToString() );

				ObjectInfoMenu.infoPosYaw.SetText( m_SelectedObject.GetOrientation()[0].ToString() );
				ObjectInfoMenu.infoPosPitch.SetText( m_SelectedObject.GetOrientation()[1].ToString() );
				ObjectInfoMenu.infoPosRoll.SetText( m_SelectedObject.GetOrientation()[2].ToString() );
			}
			else
			{
				SelectObject( obj );
				selected = true;
				Print ("Object = " + m_SelectedObject.GetType() );
				vector modelPos = building.WorldToModel( m_SelectedObject.GetPosition() );
				//PR9INICHEK: I think modelPos need to push to ObjectInfoMenu.c

				ObjectInfoMenu.infoPosX.SetText( modelPos[0].ToString() );
				ObjectInfoMenu.infoPosY.SetText( modelPos[1].ToString() );
				ObjectInfoMenu.infoPosZ.SetText( modelPos[2].ToString() );

				ObjectInfoMenu.infoPosYaw.SetText( m_SelectedObject.GetOrientation()[0].ToString() );
				ObjectInfoMenu.infoPosPitch.SetText( m_SelectedObject.GetOrientation()[1].ToString() );
				ObjectInfoMenu.infoPosRoll.SetText( m_SelectedObject.GetOrientation()[2].ToString() );
			}

		}

		if ( !selected && m_SelectedObject )
		{
			GetPlayer().MessageStatus("Current object deselected.");
			DeselectObject();
		}
	}

	void DeleteObject()
	{
		if ( !m_ObjectEditorActive )
		{
			return;
		}

		if ( m_SelectedObject )
		{
			m_Objects.RemoveItem(m_SelectedObject);
			m_SelectedObject.SetPosition(vector.Zero); // If object does not physically delete, teleport it to 0 0 0
			GetGame().ObjectDelete( m_SelectedObject );

			ObjectInfoMenu.UpdateObjectList();

			m_SelectedObject = NULL;
		}
	}

	void GroundObject()
	{
		if ( !m_ObjectEditorActive )
		{
			return;
		}

		if ( m_SelectedObject )
		{
			/*
			TStringArray sArray = new TStringArray;
			m_SelectedObject.GetSelectionList( sArray );

			foreach( string s : sArray )
			{
				Print( s );
				//GetPlayer().MessageStatus( s);
			}
			*/
			/*

			vector pos = m_SelectedObject.GetPosition();
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);

			vector clippingInfo[2];
			vector objectBBOX[2];

			m_SelectedObject.GetCollisionBox( objectBBOX );
			m_SelectedObject.ClippingInfo( clippingInfo );

			//float clipY = objectBBOX[1][1] / 2.0//- clippingInfo[0][1];
			//pos[1] = pos[1] + objectBBOX[1][1] - clipY;
			pos[1] = pos[1] + clippingInfo[1][1] / 2.0;//objectBBOX[0][1] - clipY

			m_SelectedObject.SetPosition(pos);
			*/
			//SnapToGroundNew( m_SelectedObject );
			m_SelectedObject.PlaceOnSurface();
		}
	}
}
