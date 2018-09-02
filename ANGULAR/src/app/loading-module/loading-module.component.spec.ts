import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { LoadingModuleComponent } from './loading-module.component';

describe('LoadingModuleComponent', () => {
  let component: LoadingModuleComponent;
  let fixture: ComponentFixture<LoadingModuleComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ LoadingModuleComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(LoadingModuleComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
