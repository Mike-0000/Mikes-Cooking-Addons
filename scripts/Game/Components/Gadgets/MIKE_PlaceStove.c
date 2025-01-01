[BaseContainerProps()]
class MIKE_PlaceStove : SCR_ConsumableEffectBase
{
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab to spawn when used.", params: "et")]
	ResourceName m_rnStovePrefab;
//	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Prefab to spawn when used.")]
//	EntitySpawnParams spawnParams;
	
	//------------------------------------------------------------------------------------------------	
	// Called when the user attempts to activate the effect (e.g. "use" the item).
	override bool ActivateEffect(IEntity target, IEntity user, IEntity item, ItemUseParameters animParams = null)
	{
		// We no longer care about healing or anim commands, so we can skip
		if (!user)
			return false;

		// We’ll just call our new spawn logic. If it succeeds, mark the item as "used up."
		bool didSpawn = SpawnStoveInFrontOfUser(user);
		if (!didSpawn)
			return false;
		

		ChimeraCharacter character = ChimeraCharacter.Cast(user);
	    if (!character) return false;
	
	    CharacterControllerComponent controller = character.GetCharacterController();
	    if (!controller) return false;
		
		    ItemUseParameters noAnimParams = new ItemUseParameters();
	    noAnimParams.SetEntity(item);
	    noAnimParams.SetCommandID(-1); // A command ID that doesn't exist
	    noAnimParams.SetMaxAnimLength(0.0); // zero-duration
	
	    // This effectively tells the system "use the item" without playing any real animation.
	    bool activatedAction = controller.TryUseItemOverrideParams(noAnimParams);
		super.ActivateEffect(target, user, item, noAnimParams);
		DeleteItem(user, item);
 	    return activatedAction;
		
		
		
//		// If the base class is responsible for consuming/deleting the original item, let it proceed.
//		return ;
	}

	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE){
		return true;
	}

	//------------------------------------------------------------------------------------------------
	void DeleteItem(notnull IEntity user, IEntity item)
	{
		// Normally, ApplyEffect is for healing, etc. We'll just skip or do nothing.
		// If you want the item to be removed from inventory, confirm your base class does so
		// or handle that here.
		
		
		Print("trying to delete item in hand",LogLevel.NORMAL);
		
		if (!item) return;

		// Unlock and delete the item from the world/inventory
		InventoryItemComponent itemComp = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (itemComp)
		{
			// If the item is "locked" to the user’s hands, unlock it before deleting
			itemComp.RequestUserLock(user, false);
		}

		// Remove the entity from the world
		SCR_EntityHelper.DeleteEntityAndChildren(item);
		
		
		
	}

	//------------------------------------------------------------------------------------------------
//	protected bool SpawnStoveInFrontOfUser(IEntity user)
//	{
//		if (!m_rnStovePrefab || !user)
//			return false;
//		
//		// Get the user’s transform (position + orientation) 
//		vector userTransform[4];
//		user.GetWorldTransform(userTransform);
//		
//		ref PointInfo userPos = new PointInfo();
//		userPos.Init(user);	
//		
//		vector userAngles = user.GetAngles();
//		
//		
//		float yawRad = DegToRad(userAngles[0]);
//    	float pitchRad = DegToRad(userAngles[1]);
//		float cosPitch = Math.Cos(pitchRad);
//	    float sinPitch = Math.Sin(pitchRad);
//	    float cosYaw = Math.Cos(yawRad);
//	    float sinYaw = Math.Sin(yawRad);
//		vector forward;
//    	forward[0] = cosPitch * cosYaw;  // X
//    	forward[1] = -sinPitch;          // Y
//    	forward[2] = cosPitch * sinYaw; 
//		
//		float spawnDistance = 2.0; // 2 meters in front
//		
//		
//  		vector spawnPos = userPos + (forward * spawnDistance);
//		vector spawnTransform[4];
//	    // Identity matrix
//	    spawnTransform[0] = Vector(1, 0, 0);  
//	    spawnTransform[1] = Vector(0, 1, 0);
//	    spawnTransform[2] = Vector(0, 0, 1);
//	    spawnTransform[3] = spawnPos;
//		
//		EntitySpawnParams params = EntitySpawnParams();
//		params.TransformMode = ETransformMode.WORLD;
//		
//		if (m_SpawnPosition) {
//		    m_SpawnPosition.GetModelTransform(params.Transform);
//		    params.Transform[3] = user.CoordToParent(params.Transform[3]);
//		} else {
//		    user.GetTransform(params.Transform);
//		}
//		
//		// Add a slight offset to the spawn position
//		vector offset = "-2.0 0.0 0.0"; // Adjust this as needed (X, Y, Z offsets)
//		params.Transform[3] = params.Transform[3] + offset;
//		
//		//Spawn Entity!
//		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(m_rnStovePrefab), user.GetWorld(), params);
//		
//				
//		
//		
//		
//
//		if (!entity)
//		{
//			Print("Failed to spawn stove prefab.", LogLevel.ERROR);
//			return false;
//		}
//
//		Print("Successfully spawned stove entity in front of user.", LogLevel.NORMAL);
//		return true;
//	}
static float DegToRad(float degrees)
{
    return degrees * (Math.PI / 180.0);
}

protected bool SpawnStoveInFrontOfUser(IEntity user)
{
    if (!m_rnStovePrefab || !user)
    {
        Print("SpawnStoveInFrontOfUser: Prefab or user entity is null.", LogLevel.ERROR);
        return false;
    }

    // 1. Get user's world position
    vector userPos = user.GetOrigin();
    Print("User Position: " + userPos, LogLevel.NORMAL);

    // 2. Get user's orientation angles
    vector userAngles = user.GetAngles();
    Print("User Angles (Degrees): " + userAngles, LogLevel.NORMAL);

    // 3. Correctly extract Yaw from userAngles[1]
    float yawDegrees = userAngles[1];
    float yawRad = DegToRad(yawDegrees);
    Print("User Yaw: " + yawDegrees + " degrees, " + yawRad + " radians", LogLevel.NORMAL);

    // 4. Calculate a flat forward direction (ignore pitch and roll)
    float cosYaw = Math.Cos(yawRad);
    float sinYaw = Math.Sin(yawRad);
    Print("cosYaw: " + cosYaw + ", sinYaw: " + sinYaw, LogLevel.NORMAL);

    vector forward;
    forward[0] = sinYaw;  // X component
    forward[1] = 0.0;     // Y component remains unchanged to avoid sinking underground
    forward[2] = cosYaw;  // Z component
    Print("Forward Vector: " + forward, LogLevel.NORMAL);

    // 5. Define the spawn distance
    float spawnDistance = 1.5; // 2 meters in front
    Print("Spawn Distance: " + spawnDistance, LogLevel.NORMAL);

    // 6. Calculate spawn position by adding forward vector scaled by spawn distance
    vector spawnOffset;
    spawnOffset[0] = forward[0] * spawnDistance;
    spawnOffset[1] = forward[1] * spawnDistance;
    spawnOffset[2] = forward[2] * spawnDistance;
    Print("Spawn Offset: " + spawnOffset, LogLevel.NORMAL);

    vector spawnPos;
    spawnPos[0] = userPos[0] + spawnOffset[0];
    spawnPos[1] = userPos[1]; // Keep the same Y position
    spawnPos[2] = userPos[2] + spawnOffset[2];
    Print("Calculated Spawn Position: " + spawnPos, LogLevel.NORMAL);

    // 7. Construct the rotation matrix based on yaw
    vector spawnTransform[4];

    // Right Vector: <cosYaw, 0.0, -sinYaw>
    spawnTransform[0] = Vector(cosYaw, 0.0, -sinYaw); // "right" axis
    Print("Spawn Transform Right Vector: " + spawnTransform[0], LogLevel.NORMAL);

    // Up Vector remains unchanged
    spawnTransform[1] = Vector(0.0, 1.0, 0.0); // "up" axis
    Print("Spawn Transform Up Vector: " + spawnTransform[1], LogLevel.NORMAL);

    // Forward Vector: <sinYaw, 0.0, cosYaw>
    spawnTransform[2] = Vector(sinYaw, 0.0, cosYaw); // "forward" axis
    Print("Spawn Transform Forward Vector: " + spawnTransform[2], LogLevel.NORMAL);

    // Position Vector
    spawnTransform[3] = spawnPos;
    Print("Spawn Transform Position: " + spawnTransform[3], LogLevel.NORMAL);

    // 8. Rotate the object 180 degrees around its own Y-axis
    // This is done by negating the X and Z components of the Right and Forward vectors
    // Since we're dealing with indices, we'll access components by [0], [1], [2]
    spawnTransform[0][0] = -spawnTransform[0][0]; // Negate X of Right Vector
    spawnTransform[0][2] = -spawnTransform[0][2]; // Negate Z of Right Vector

    spawnTransform[2][0] = -spawnTransform[2][0]; // Negate X of Forward Vector
    spawnTransform[2][2] = -spawnTransform[2][2]; // Negate Z of Forward Vector

    Print("After 180-degree Rotation:");
    Print("Spawn Transform Right Vector: " + spawnTransform[0], LogLevel.NORMAL);
    Print("Spawn Transform Forward Vector: " + spawnTransform[2], LogLevel.NORMAL);

    // 9. Set up the spawn parameters
    EntitySpawnParams params = EntitySpawnParams();
    params.TransformMode = ETransformMode.WORLD;
    params.Transform = spawnTransform;
    Print("EntitySpawnParams.TransformMode set to WORLD.", LogLevel.NORMAL);
    Print("EntitySpawnParams.Transform: " + params.Transform, LogLevel.NORMAL);

    // 10. Spawn the entity
    IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(m_rnStovePrefab), user.GetWorld(), params);
    if (!entity)
    {
        Print("SpawnStoveInFrontOfUser: Failed to spawn stove prefab.", LogLevel.ERROR);
        return false;
    }

    Print("Successfully spawned stove entity at position: " + spawnPos, LogLevel.NORMAL);
    Print("Spawned Entity Rotation - Right: " + spawnTransform[0] + ", Up: " + spawnTransform[1] + ", Forward: " + spawnTransform[2], LogLevel.NORMAL);
    return true;
}






	//------------------------------------------------------------------------------------------------
	override bool UpdateAnimationCommands(IEntity user)
	{
		// If you have no animations to play, you can return false.
		// Or just remove this override entirely if the base class doesn't need it.
		return false;
	}
}
