local events = require("devilutionx.events")

events.registerCustom("NoSpawnCondition")
events.registerCustom("RewardSpawnCondition")
events.registerCustom("VendorSpawnCondition")
events.registerCustom("SmithPremiumHfSpawnCondition")
events.registerCustom("BoyHfSpawnCondition")

events.NoSpawnCondition.add(function(eventData)
    eventData.handled = true
    return false
end)

events.RewardSpawnCondition.add(function(eventData)
    eventData.handled = true
    return eventData.item._iCurs == eventData.iCurs
end)

events.VendorSpawnCondition.add(function(eventData)
    print(string.format("[Lua] Checking VendorSpawnCondition: _iCurs=%d, _iIvalue=%d, _iSpell=%d",
        eventData.item._iCurs, eventData.item._iIvalue, eventData.item._iSpell))

    if eventData.item._iIvalue > 500 then
        eventData.handled = true
    end
end)

print("[Lua] Registering VendorSpawnCondition in", tostring(events))

events.SmithPremiumHfSpawnCondition.add(function(eventData)
    if eventData.item._iIvalue <= eventData.maxValue
        and eventData.item._iMinStr <= eventData.strength
        and eventData.item._iMinMag <= eventData.magic
        and eventData.item._iMinDex <= eventData.dexterity
        and eventData.item._iIvalue >= eventData.ivalue then
        eventData.handled = true
    end
end)

events.BoyHfSpawnCondition.add(function(eventData)
    if eventData.item._iIvalue > eventData.maxValue
        or eventData.item._iMinStr > eventData.strength
        or eventData.item._iMinMag > eventData.magic
        or eventData.item._iMinDex > eventData.dexterity
        or eventData.item._iIvalue < eventData.ivalue
        or eventData.count < eventData.maxCount then
        eventData.handled = true
    end
end)
