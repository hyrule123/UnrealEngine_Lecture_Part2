// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/ABWidgetComponent.h"

#include "UI/ABUserWidget.h"

void UABWidgetComponent::InitWidget()
{
	Super::InitWidget();

	//호출 시점: Widget 인스턴스들이 생성된 직후
	//여기서 ABUserWidget::SetOwningActor() 함수를 호출한다.
	UABUserWidget* ABUserWidget = Cast<UABUserWidget>(GetWidget());
	if (ABUserWidget)
	{
		//Widget Component를 소유한 컴포넌트 주소를 위젯에 전달해 준다.
		ABUserWidget->SetOwningActor(GetOwner());
	}
}
