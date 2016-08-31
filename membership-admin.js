Drupal.behaviors.membership_admin = function(context) {
  // Set up click events on the display selector checkboxes.
  $.each(['status', 'identity', 'contact', 'emergency'], function(index, value) {
    $('#' + value, context).click(function() {
      if ($(this).attr('checked')) {
        $('.' + value).show();
      }
      else {
        $('.' + value).hide();
      }
    });
    $('.status').hide();
    $('.emergency').hide();
    $('#contact').attr('checked', 'checked');
    $('#identity').attr('checked', 'checked');
  });

  // Set up select-all
  $('#select-all', context).click(function() {
    if ($(this).attr('checked')) {
      $('.selection').attr('checked', 'checked');
    }
    else {
      $('.selection').removeAttr('checked');
    }
    checkActionDropdownState();
  });

  // Set up unselect all when one checkbox is unselected.
  $('.selection', context).click(function() {
    if (!$(this).attr('checked')) {
      $('#select-all').removeAttr('checked');
    }
    checkActionDropdownState();
  });

  // Disable the 'Action' dropdown if there is no selection.
  checkActionDropdownState();

  // Set up the actions on selected records.
  $('#run-action', context).click(function() {
    var command = $('#action').val();
    // Assemble a list of ids to send to the server with the command:
    var selected_members = $.map($('.selection:checked', context), function(element) {
      return element.value;
    });
    alert("Running " + command + " on selection of " + selected_members.join(','));
    var post_data = {
      // filter: filter,
      selected_members: selected_members.join(',')
    };

    $.post('/membership/update/' + command, post_data, function(result) {
      eval("output=" + result);
      alert("Ran " + command + " to get " + output.result + " with " + output.data.selected_members);
    });
  });
}

function checkActionDropdownState() {
  $('#action').attr('title', Drupal.t('Select the records you want to modify by checking their checkboxes.'));
  $('#action').attr('disabled', true);

  $('.selection').each(function() {
    if ($(this).attr('checked')) {
      $('#action').attr('disabled', false);
    }
  });

  $('#action:enabled').attr('title', Drupal.t('Choose your action then click the Run button.'));
  if ($('#action:enabled').size()) {
    $('#run-action').attr('disabled', false).attr('title', Drupal.t('Click here to run the chosen action against all the checked records below.'));
  }
  else {
    $('#run-action').attr('disabled', true).attr('title', Drupal.t('Choose an action to run first.'));
  }
}
