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
	protected bool SpawnStoveInFrontOfUser(IEntity user)
	{
		if (!m_rnStovePrefab || !user)
			return false;
		
		// Get the user’s transform (position + orientation) 
		vector userTransform[4];
		user.GetWorldTransform(userTransform);

		
		ref PointInfo m_SpawnPosition = new PointInfo();
		m_SpawnPosition.Init(user);	
		
		
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		if (m_SpawnPosition) {
		    m_SpawnPosition.GetModelTransform(params.Transform);
		    params.Transform[3] = user.CoordToParent(params.Transform[3]);
		} else {
		    user.GetTransform(params.Transform);
		}
		
		// Add a slight offset to the spawn position
		vector offset = "-2.0 0.0 0.0"; // Adjust this as needed (X, Y, Z offsets)
		params.Transform[3] = params.Transform[3] + offset;
		
		//Spawn Entity!
		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(m_rnStovePrefab), user.GetWorld(), params);
		
				
		
		
		

		if (!entity)
		{
			Print("Failed to spawn stove prefab.", LogLevel.ERROR);
			return false;
		}

		Print("Successfully spawned stove entity in front of user.", LogLevel.NORMAL);
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
